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

    MsgQueue *mqdesc = (MsgQueue*) desc->resource;

    List_detach(&(mqdesc->resource.descriptors_ptrs), (ListItem *) descPtr);
    List_detach(&(running->descriptors), (ListItem *) desc);

    if (DescriptorPtr_free(descPtr) != 0) {
        printf("[ERROR] Failed to deallocate the descriptor ptr (fd = %d)!\n", fd);
        running->syscall_retvalue = DSOS_EMQ_CLOSE;
        return;
    }
    if (Descriptor_free(desc) != 0) {
        printf("[ERROR] Failed to deallocate the descriptor with fd %d!\n", fd);
        running->syscall_retvalue = DSOS_EMQ_CLOSE;
        return;
    }
    //printf("Message queue (fd %d) closed!\n", fd);

    // La coda verrà distrutta una volta che tutti i processi che l'hanno aperta chiudono
    // i propri descrittori associati ad essa

    if (mqdesc->resource.descriptors_ptrs.size == 0) {
        disastrOS_debug("All descriptors are closed: msg queue (fd = %d) unlinked!\n", fd);

        running->syscall_args[0] = (long) mqdesc->resource.name;
        internal_msgQueueUnlink();
        if (running->syscall_retvalue == DSOS_EMQ_CLOSE)
            return;
    }
    disastrOS_debug("Message queue (fd = %d) closed and unlinked\n", fd);
    running->syscall_retvalue = 0;

}
