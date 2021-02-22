#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"

// Prende in input il nome della coda di messaggi da creare
void internal_msgQueueCreate() {

    const char *name = (const char*)running->syscall_args[0];

    MsgQueue *mqdes = MsgQueue_alloc(name, last_rid++, running);
    MsgQueuePtr *mqPtr = MsgQueuePtr_alloc(mqdes);

    List_insert(&resources_list, resources_list.last, (ListItem *)mqdes);
    List_insert(&msg_queues_list, msg_queues_list.last, (ListItem *)mqPtr);

    disastrOS_debug("Message queue with name '%s' created!\n", name);

    running->syscall_retvalue = 0;
}

