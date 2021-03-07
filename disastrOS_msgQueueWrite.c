#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_globals.h"

void internal_msgQueueWrite() {

    int mqdes = (int) running->syscall_args[0];
    const char *msg_ptr = (const char *) running->syscall_args[1];
    unsigned int msg_len = (unsigned int) running->syscall_args[2];
    unsigned int priority = (unsigned int) running->syscall_args[3];

    Descriptor *desc = DescriptorList_byFd(&running->descriptors, mqdes);

    if (!desc) {
        printf("[ERROR] The message queue (fd = %d) isn't opened for the running process!\n", mqdes);
        running->syscall_retvalue = DSOS_EMQ_WRITE;
        return;
    }

    MsgQueue *mq = (MsgQueue*)desc->resource;

    if (mq->size == MAX_NUM_MESSAGES_PER_MSG_QUEUE) { // coda piena
        printf("[ERROR] Unable to write msg: the msg queue (fd =% d) is full!\n", mqdes);

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

        old_running->syscall_retvalue = 0;
        printf("[WRITE] Terminated after blocking the running process \n\n");
        return;
    }

    if (msg_len > MAX_TEXT_LEN) {   // messaggio troppo grande
        printf("[ERROR] The maximum message length is %d!\n", MAX_TEXT_LEN);
        running->syscall_retvalue = DSOS_EMQ_WRITE;
        return;
    }

    Subqueue *msgSubqueue = mq->subqueues[priority];

    printf(">> Allocating a new msg message in the subqueue with priority %u\n", priority);
    Message *msg = Message_alloc(msg_ptr, msg_len);

    if (!msg) {
        printf("[ERROR] Failed to allocate new message!\n");
        running->syscall_retvalue = DSOS_EMQ_WRITE;
        return;
    }
    // inserisco (scrivo) messaggio nella lista di messaggi della sottocoda con quella priorità
    List_insert(&(msgSubqueue->messages), msgSubqueue->messages.last, (ListItem *) msg);
    ++(mq->size);

    printf(">> Message '%s' written!\n\n", msg->msg_ptr);

    if (mq->waiting_descriptors.size > 0) {
        printf("[WAKE] Ci sono processi in waiting descriptors da risvegliare\n");

        DescriptorPtr *descPtr_next = (DescriptorPtr*)List_detach(&mq->waiting_descriptors, mq->waiting_descriptors.first);
        printf("[WAKE] Descrittore tolto da waiting descriptors: pid = %d \n", descPtr_next->descriptor->pcb->pid);

        PCB* next_pcb = descPtr_next->descriptor->pcb;
        next_pcb->status = Ready;

        List_detach((ListHead*)&waiting_list, (ListItem*)next_pcb);
        printf("[WAKE] Processo tolto dalla waiting list: pid = %d \n", next_pcb->pid);

        //disastrOS_printStatus();

        List_insert((ListHead*)&ready_list, (ListItem*)ready_list.last, (ListItem*)next_pcb);
        printf("[WAKE] Processo messo in ready: pid = %d \n", next_pcb->pid);

        DescriptorPtr_free(descPtr_next);

        //disastrOS_printStatus();

        //PCB* pcb_next = (PCB*)List_detach(&ready_list, ready_list.first);
        //running = pcb_next;

    } else {
        printf("\n Non ci sono processi bloccati in waiting descriptors\n");
    }

    running->syscall_retvalue = msg_len;
}
