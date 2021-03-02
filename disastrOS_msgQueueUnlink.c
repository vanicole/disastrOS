#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_descriptor.h"

void internal_msgQueueUnlink() {

    const char *name = (const char *) running->syscall_args[0];

    // ricerca ptr alla msg queue tramite il nome
    MsgQueuePtr *mqPtr = MsgQueuePtrList_findByName(&msg_queues_list, name);
    MsgQueue *mqdesc = NULL;

    if (mqPtr != NULL) {
        mqdesc = mqPtr->msgQueuePtr;
    }

    if (!mqdesc) {
        printf("[ERROR] Now doesn't exist message queue with name '%s'! It has already been unlinked!\n\n", name);
        running->syscall_retvalue = DSOS_EMQ_UNLINK;
        return;
    }

    // La coda verrà distrutta una volta che tutti i processi che l'hanno
    // aperta chiudono i propri descrittori associati alla coda
    if (mqdesc->resource.descriptors_ptrs.size > 0) {
        printf("[ERROR] The message queue '%s' has opened descriptors!\n", name);
        running->syscall_retvalue = 0;
        return;
    }

    // rimuove ptr a msg queue da lista msg_queues_list
    List_detach(&msg_queues_list, (ListItem *) mqPtr);
    // rimuove descrittore associato alla coda da resources_list
    List_detach(&resources_list, (ListItem *) mqdesc);

    MsgQueuePtr_free(mqPtr);
    MsgQueue_free(mqdesc);

    disastrOS_debug("Message queue with name '%s' destroyed!\n", name);
    running->syscall_retvalue = 0;

}
