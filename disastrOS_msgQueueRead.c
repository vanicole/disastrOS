#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"

void internal_msgQueueRead() {

    int mqdes = (int) running->syscall_args[0];
    char *msg_ptr = (char *) running->syscall_args[1];
    unsigned int msg_len = (unsigned) running->syscall_args[2]; // 256

    Descriptor *desc = DescriptorList_byFd(&(running->descriptors), mqdes);

    if (!desc) {
        running->syscall_retvalue = DSOS_EMQ_READ;
        return;
    }

    MsgQueue *mq = (MsgQueue *)desc->resource;
    Subqueue *msgSubqueue = NULL;

    // legge messaggi in ordine di priorità
    unsigned int priority;
    for (priority = 0; priority < MAX_NUM_PRIORITIES; ++priority) {
        if (mq->subqueues[priority]->messages.size > 0) {  // se ci sono messaggi nella coda
            msgSubqueue = mq->subqueues[priority];
            break;
        }
    }


    // se non ci sono messaggi da leggere: metto processo running in waiting descriptors
    if (!msgSubqueue) {
        printf("[ERROR] Unable to read msg: the msg queue (fd =% d) is empty!\n", mqdes);

        PCB* old_running = running;
        running->status = Waiting;
        printf("[WAIT] process (pid = %d) -> from Running to Waiting \n", running->pid);

        // alloco il ptr alla coda per inserirlo nella lista di descrittori in waiting
        DescriptorPtr *descPtr = DescriptorPtr_alloc(desc);
        assert(descPtr);

        List_insert((ListHead*)&mq->waiting_descriptors, (ListItem*)mq->waiting_descriptors.last, (ListItem*)descPtr);
        printf("[WAIT] Inserimento processo running (pid = %d) in waiting descriptors\n", descPtr->descriptor->pcb->pid);


        // modifica
        List_insert((ListHead*)&waiting_list, (ListItem*)waiting_list.last, (ListItem*)running);
        printf("[WAIT] Inserimento processo running (pid = %d) in waiting list\n", running->pid);

    /*
        PCB* pcb_next = (PCB*)List_detach(&ready_list, ready_list.first);
        printf("[WAIT] Rimozione processo (pid = %d) da ready list \n", pcb_next->pid);
        running = pcb_next;
        printf("[WAIT] Imposto processo (pid = %d) in running\n", running->pid);
    */

        //disastrOS_printStatus();
        old_running->syscall_retvalue = 0;
        printf("[READ] Terminated after blocking the running process \n\n");

        return;
    }


    // prendo il primo messaggio della lista di messaggi d
    Message *msg = (Message *)msgSubqueue->messages.first;

    // verifico dimensione messaggio
    if (msg->msg_len > msg_len) {
        printf("[ERROR] The message length %d must be greater than %d!\n", msg_len, msg->msg_len);
        running->syscall_retvalue = DSOS_EMQ_READ;
        return;
    }
    // inserisce msg->msg_ptr nel buffer puntato da msg_ptr
    strcpy(msg_ptr, msg->msg_ptr);


    // rimuovo messaggio da lista di messaggi della sottocoda e decremento la size
    List_detach(&(msgSubqueue->messages), (ListItem *) msg);
    --(mq->size);

    printf(">> Message '%s' read!\n", msg->msg_ptr);
    Message_free(msg);
    printf("[READ] Terminated correctly!\n");
    running->syscall_retvalue = msg->msg_len;
}
