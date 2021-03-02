#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"

void internal_msgQueueOpen() {

    //1 get from the PCB the name of the resource to open
    const char *name = (const char*) running->syscall_args[0];

    MsgQueuePtr *mqdesc = MsgQueuePtrList_findByName(&msg_queues_list, name);
    MsgQueue *mq = mqdesc->msgQueuePtr;

    if (!mq) {
        running->syscall_retvalue = DSOS_EMQ_NOEXIST;
        return;
    }
    // create the descriptor for the resource in this process, and add it to
    // the process descriptor list. Assign to the resource a new fd

    //printf("Allocation the descriptor fd = %d \n", running->last_fd);
    Descriptor *desc = Descriptor_alloc(running->last_fd, (Resource*) mq, running);
    if (!desc) {
        running->syscall_retvalue = DSOS_EMQ_NOFD;
        return;
    }

    running->last_fd++; // we increment the fd value for the next call
    DescriptorPtr *descPtr = DescriptorPtr_alloc(desc);

    // inserisce il nuovo descrittore nella lista dei descrittori aperti del PCB
    List_insert(&running->descriptors, running->descriptors.last, (ListItem *) desc);

    // add to the resource, in the descriptor ptr list, a pointer to the newly created descriptor
    desc->ptr = descPtr;
    List_insert(&(mq->resource.descriptors_ptrs), mq->resource.descriptors_ptrs.last, (ListItem *) descPtr);

    //printf("Message queue with name '%s' opened!\n", name);

    // return the FD of the new descriptor to the process
    running->syscall_retvalue = desc->fd;

}
