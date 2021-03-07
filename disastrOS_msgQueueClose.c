#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"

void internal_msgQueueClose() {
    int fd = running->syscall_args[0];

    Descriptor *desc = DescriptorList_byFd(&running->descriptors, fd);
    DescriptorPtr *descPtr = desc->ptr;

  // message queue is already closed
    if (!desc) {
        printf("[ERROR] Msg queue (fd = %d) already closed!\n", fd);
        running->syscall_retvalue = 0;
        return;
    }

    MsgQueue *mqdesc = (MsgQueue*)desc->resource;

    List_detach(&mqdesc->resource.descriptors_ptrs, (ListItem *) descPtr);
    List_detach(&running->descriptors, (ListItem *) desc);

    if (DescriptorPtr_free(descPtr) != 0) {
        disastrOS_debug("[ERROR] Failed to deallocate the descriptor ptr (fd = %d)!\n", fd);
        running->syscall_retvalue = DSOS_EMQ_CLOSE;
        return;
    }
    if (Descriptor_free(desc) != 0) {
        disastrOS_debug("[ERROR] Failed to deallocate the descriptor with fd %d!\n", fd);
        running->syscall_retvalue = DSOS_EMQ_CLOSE;
        return;
    }

    // La coda verrà distrutta una volta che tutti i processi che l'hanno aperta chiudono
    // i propri descrittori associati ad essa
    if (mqdesc->resource.descriptors_ptrs.size == 0) {
        printf("All descriptors associated with the message queue have been closed: the queue will be unlinked!\n\n");
        running->syscall_args[0] = (long) mqdesc->resource.name;
        internal_msgQueueUnlink();
        printf("Message queue (fd = %d) closed and unlinked\n", fd);
        if (running->syscall_retvalue == DSOS_EMQ_CLOSE)
            return;
    }
    disastrOS_debug("Message queue (fd = %d) closed and unlinked\n", fd);

    if (mqdesc->waiting_descriptors.size > 0) {
        printf("[WAKE BEFORE CLOSE] Ci sono processi in waiting descriptors da risvegliare prima di terminare\n");

        DescriptorPtr *descPtr_next = (DescriptorPtr*)List_detach(&mqdesc->waiting_descriptors, mqdesc->waiting_descriptors.first);
        printf("[WAKE BEFORE CLOSE] Descrittore tolto da waiting descriptors: pid = %d \n", descPtr_next->descriptor->pcb->pid);

        PCB* next_pcb = descPtr_next->descriptor->pcb;
        next_pcb->status = Ready;

        List_detach((ListHead*)&waiting_list, (ListItem*)next_pcb);
        printf("[WAKE BEFORE CLOSE] Processo tolto dalla waiting list: pid = %d \n", next_pcb->pid);

        //disastrOS_printStatus();

        List_insert((ListHead*)&ready_list, (ListItem*)ready_list.last, (ListItem*)next_pcb);
        printf("[WAKE BEFORE CLOSE] Processo messo in ready: pid = %d \n", next_pcb->pid);

        DescriptorPtr_free(descPtr_next);

    }

    running->syscall_retvalue = 0;

}
