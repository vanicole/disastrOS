#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"


void internal_msgQueueCreate() {
   // get from the PCB the name of the resource to create
    const char *name = (const char*)running->syscall_args[0];
    int rid = last_rid++;

    if (!name) {
        printf("[ERROR] 'name' must be different from NULL!\n");
        running->syscall_retvalue = DSOS_EMQ_CREATE;
        return;
    }

    // Allocazione coda di messaggi da parte del processo running
    MsgQueue *mqdes = MsgQueue_alloc(name, rid, running);
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

    printf(">> Inserted message queue (rid = %d) in resources list\n", rid);
    List_insert(&resources_list, resources_list.last, (ListItem *)mqdes);
    printf(">> Inserted ptr to the msg queue in msg queues list\n");
    List_insert(&msg_queues_list, msg_queues_list.last, (ListItem *)mqPtr);

    //printf("Message queue with name '%s' created!\n", name);

    running->syscall_retvalue = 0;
}
