#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"


void internal_msgQueueCreate() {

    const char *name = (const char*)running->syscall_args[0];

    if (name == NULL) {
        printf("[ERROR] 'name' must be different from NULL!\n");
        running->syscall_retvalue = DSOS_EMQ_CREATE;
        return;
    }

    MsgQueue *mqdes = MsgQueue_alloc(name, last_rid++, running);
    if (!mqdes) {
        printf("[ERROR] Unable to allocate a new msg queue %s\n", name);
        running->syscall_retvalue = DSOS_EMQ_CREATE;
        return;
    }

    MsgQueuePtr *mqPtr = MsgQueuePtr_alloc(mqdes);
    if (!mqPtr) {
        printf("[ERROR] Unable to allocate message queue ptr with name %s \n", name);
        running->syscall_retvalue = DSOS_EMQ_CREATE;
        return;
    }

    printf("Inserimento di mqdes nella resources list\n");
    List_insert(&resources_list, resources_list.last, (ListItem *)mqdes);
    printf("Inserimento del ptr alla msg queue nella msg queues list\n");
    List_insert(&msg_queues_list, msg_queues_list.last, (ListItem *)mqPtr);

    disastrOS_debug("Message queue with name '%s' created!\n", name);

    running->syscall_retvalue = 0;
}
