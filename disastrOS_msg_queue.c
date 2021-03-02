#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pool_allocator.h"
#include "disastrOS.h"
#include "disastrOS_msg_queue.h"
#include "disastrOS_constants.h"
#include "linked_list.h"

#define DSOS_MSG_QUEUE_RESOURCE 3 // tipo di risorsa coda

// macro associate al Text del messaggio
#define TEXT_SIZE sizeof((MAX_TEXT_LEN + 1) * sizeof(char))
#define TEXT_MEM_SIZE (TEXT_SIZE + sizeof(int))
#define TEXT_BUFFER_SIZE MAX_NUM_MESSAGES * TEXT_MEM_SIZE

// macro associate al Message
#define MESSAGE_SIZE sizeof(Message)
#define MESSAGE_MEM_SIZE (sizeof(Message) + sizeof(int))
#define MESSAGE_BUFFER_SIZE MAX_NUM_MESSAGES * MESSAGE_MEM_SIZE

// macro associate a Subqueue
#define SUBQUEUE_SIZE sizeof(Subqueue)
#define SUBQUEUE_MEM_SIZE (sizeof(Subqueue) + sizeof(int))
#define SUBQUEUE_BUFFER_SIZE MAX_NUM_PRIORITIES * MAX_NUM_RESOURCES * SUBQUEUE_MEM_SIZE

// macro associate alla MsgQueue
#define MSG_QUEUE_SIZE sizeof(MsgQueue)
#define MSG_QUEUE_MEM_SIZE (MSG_QUEUE_SIZE + sizeof(int))
#define MSG_QUEUE_BUFFER_SIZE MAX_NUM_RESOURCES * MSG_QUEUE_MEM_SIZE

// macro associate a MsgQueuePtr
#define MSG_QUEUE_PTR_SIZE sizeof(MsgQueuePtr)
#define MSG_QUEUE_PTR_MEM_SIZE (sizeof(MsgQueuePtr) + sizeof(int))
#define MSG_QUEUE_PTR_BUFFER_SIZE MAX_NUM_RESOURCES * MSG_QUEUE_PTR_MEM_SIZE

// pool allocator (basato su disastrOS_resource.c)
static char _texts_buffer[TEXT_BUFFER_SIZE];
static PoolAllocator _texts_allocator;

static char _messages_buffer[MESSAGE_BUFFER_SIZE];
static PoolAllocator _messages_allocator;

static char _subqueues_buffer[SUBQUEUE_BUFFER_SIZE];
static PoolAllocator _subqueues_allocator;

static char _msg_queues_buffer[MSG_QUEUE_BUFFER_SIZE];
static PoolAllocator _msg_queues_allocator;

static char _msg_queue_ptrs_buffer[MSG_QUEUE_PTR_BUFFER_SIZE];
static PoolAllocator _msg_queue_ptrs_allocator;


/****************** FUNZIONI ASSOCIATE A text: init, alloc e free *************/

// void Resource_init()
void Text_init() {
    PoolAllocatorResult res = PoolAllocator_init(&_texts_allocator,
                                                TEXT_SIZE,
                                                MAX_NUM_MESSAGES,
                                                _texts_buffer,
                                                TEXT_BUFFER_SIZE);

    if (res != Success) {
        printf("[ERROR] Failed initialize text allocator!\n");
        assert(res == Success);
    }
}

// Resource* Resource_alloc(int id, int type)
char* Text_alloc() {
    char* s = (char*) PoolAllocator_getBlock(&_texts_allocator);
    memset(s, 0, TEXT_MEM_SIZE);
    return s;
}

// int Resource_free(Resource* r)
int Text_free(char *s) {
    PoolAllocatorResult res = PoolAllocator_releaseBlock(&_texts_allocator, s);
    if (res != Success){
        printf("[ERROR] Failed to deallocate text allocator!\n");
        return -1;
    }
    return 0;
}


/***************** FUNZIONI ASSOCIATE A MESSAGE: init, alloc, free, print e printList ***************/

// void Resource_init()
void Message_init() {
    PoolAllocatorResult res = PoolAllocator_init(&_messages_allocator,
                                                MESSAGE_SIZE,
                                                MAX_NUM_MESSAGES,
                                                _messages_buffer,
                                                MESSAGE_BUFFER_SIZE);

    if (res != Success) {
        printf("[ERROR] Failed initialize messages allocator!\n");
        assert(res == Success);
    }
}

// Resource* Resource_alloc(int id, int type)
Message* Message_alloc(const char *msg, unsigned size) {
    char *txt = Text_alloc();
    Message *new_msg = (Message*) PoolAllocator_getBlock(&_messages_allocator);

    if (!new_msg) {
        printf("[ERROR] Failed to allocate the message %s!\n", txt);
        Text_free(txt);
        return NULL;
    }

    new_msg->list.prev = new_msg->list.next = NULL;     // r->list.prev = r->list.next = 0;
    strcpy(txt, msg);
    new_msg->msg_ptr = txt;                             // r->id=id;
    new_msg->msg_len = size;                            // r->type=type;

    printf(">> Message '%s' allocated correctly!\n", txt);
    return new_msg;
}

// int Resource_free(Resource* r)
int Message_free(Message *msg) {
    PoolAllocatorResult res = PoolAllocator_releaseBlock(&_messages_allocator, msg);
    if (res != Success) {
        printf("[ERROR] Failed to deallocate message %s!\n", msg->msg_ptr);
        return -1;
    }
    printf(">> Message '%s' deallocated correctly!\n", msg->msg_ptr);
    return 0;
}

// void Resource_print(Resource* r)
void Message_print(const Message *msg) {
    if (msg == NULL) printf("[ ]");
    printf("(msg: '%s', size: %d) ", msg->msg_ptr, msg->msg_len);
}

// ResourceList_print(ListHead* l)
void MessageList_print(MessageList *list) {
    printf("{");
    ListItem* aux = list->first;
    while(aux) {
        Message *msg = (Message*)aux;
        Message_print(msg);
        if (aux->next != NULL)
            printf(",");
        aux = aux->next;
    }
    printf("}");
}



/****** FUNZIONI ASSOCIATE A MSG PRIORITY SUBQUEUE: init, alloc, free, print *********/

// void Resource_init()
void Subqueue_init() {
    PoolAllocatorResult res = PoolAllocator_init(&_subqueues_allocator,
                                                SUBQUEUE_SIZE,
                                                MAX_NUM_PRIORITIES * MAX_NUM_RESOURCES, // MAX_NUM_PRIORITIES_RESOURCE
                                                _subqueues_buffer,
                                                SUBQUEUE_BUFFER_SIZE);

    if (res != Success) {
        printf("[ERROR] Failed initialize message subqueues allocator!\n");
        assert(res == Success);
    }
}

// Resource* Resource_alloc(int id, int type)
Subqueue* Subqueue_alloc(unsigned priority) {
    Subqueue *m = (Subqueue *)PoolAllocator_getBlock(&_subqueues_allocator);
    m->list.prev = m->list.next = NULL;
    List_init(&(m->messages));              // List_init(&r->descriptors_ptrs);
    m->priority = priority;
    printf(">> Message subqueue with priority %d allocated correctly!\n", priority);
    return m;
}

// int Resource_free(Resource* r)
int Subqueue_free(Subqueue* msg) {
    for (ListItem *currMsg = msg->messages.first; currMsg != NULL;) {
        Message *oldMsg = (Message*)currMsg;
        currMsg = currMsg->next;
        List_detach(&msg->messages, (ListItem*)oldMsg);
        Message_free(oldMsg);   // dealloca eventuali messaggi ancora presenti nella coda
    }

    PoolAllocatorResult res = PoolAllocator_releaseBlock(&_subqueues_allocator, msg);
    if (res != Success) {
        printf("[ERROR] Failed to deallocate message subqueue!\n");
        return -1;
    }
    return 0;
}

// void Resource_print(Resource* r)
void Subqueue_print(const Subqueue* msg) {
    if (msg == NULL)
        printf("[NULL]");
    else {
        printf("\n\t\t {priority: %d, messages: ", msg->priority);
        MessageList_print((ListHead*) & (msg->messages));
        printf(", count: %d}", msg->messages.size);
    }
}


/******** FUNZIONI ASSOCIATE A MSG QUEUE: init, alloc, free, print *******/

// void Resource_init()
void MsgQueue_init() {
    PoolAllocatorResult res = PoolAllocator_init(&_msg_queues_allocator,
                                                MSG_QUEUE_SIZE,
                                                MAX_NUM_RESOURCES,
                                                _msg_queues_buffer,
                                                MSG_QUEUE_BUFFER_SIZE);

    if (res != Success) {
        printf("[ERROR] Failed initialize message queues allocator!\n");
        assert(res == Success);
    }
}


// Resource* Resource_alloc(int id, int type)
MsgQueue* MsgQueue_alloc(const char *name, int id, PCB *pcb) {
    MsgQueue *q = (MsgQueue*)PoolAllocator_getBlock(&_msg_queues_allocator);
    q->resource.list.prev = q->resource.list.next = NULL;
    q->resource.name = name;
    q->resource.rid = id;
    q->resource.type = DSOS_MSG_QUEUE_RESOURCE; // impostato a 3 senza un motivo specifico

    List_init(&(q->resource.descriptors_ptrs));
    for (unsigned int priority = 0; priority < MAX_NUM_PRIORITIES; ++priority) {
        q->subqueues[priority] = Subqueue_alloc(priority);
    }

    q->pcb = pcb;
    q->size = 0;
    printf(">> Message queue with name %s allocated correctly!\n", name);
    return q;
}

// int Resource_free(Resource* resource)
int MsgQueue_free(MsgQueue *q) {
    for (unsigned int priority = 0; priority < MAX_NUM_PRIORITIES; ++priority) {
        Subqueue_free(q->subqueues[priority]);
        printf(">> Subqueue with priority %d deallocated correctly!\n", priority);
    }
    PoolAllocatorResult res = PoolAllocator_releaseBlock(&_msg_queues_allocator, q);
    if (res != Success) {
        printf("[ERROR] Failed to deallocate message queue!\n");
        return -1;
    }
    return 0;
}

// void Resource_print(Resource* r)
void MsgQueue_print(const MsgQueue *q) {
    if (q == NULL)
        printf("[NULL]");
    else {
        printf("[name: '%s', \n\t resource id: %d, \n\t descriptors: ", q->resource.name, q->resource.rid);
        DescriptorPtrList_print((ListHead *) &(q->resource.descriptors_ptrs));
        printf(",  \n\t subqueues: ");
        for (unsigned priority = 0; priority < MAX_NUM_PRIORITIES; ++priority) {
            Subqueue_print(q->subqueues[priority]);
            if (priority + 1 < MAX_NUM_PRIORITIES)
                printf(",\n");
        }
        printf("\n\t]");
    }
}


/**** FUNZIONI ASSOCIATE A MSGQUEUE PTR: init, alloc, free, print, printList ********/

// void Resource_init()
void MsgQueuePtr_init() {
    PoolAllocatorResult res = PoolAllocator_init(&_msg_queue_ptrs_allocator,
                                                MSG_QUEUE_PTR_SIZE,
                                                MAX_NUM_RESOURCES,
                                                _msg_queue_ptrs_buffer,
                                                MSG_QUEUE_PTR_BUFFER_SIZE);

    if (res != Success) {
        printf("[E0RROR] Failed initialize message queue ptr allocator!\n");
        assert(res == Success);
    }
}

// Resource* Resource_alloc(int id, int type);
MsgQueuePtr* MsgQueuePtr_alloc(MsgQueue *q) {
    MsgQueuePtr *mq = (MsgQueuePtr*)PoolAllocator_getBlock(&_msg_queue_ptrs_allocator);
    mq->msgQueuePtr = q;
    return mq;
}

// int Resource_free(Resource* resource)
int MsgQueuePtr_free(MsgQueuePtr *q) {
    PoolAllocatorResult res = PoolAllocator_releaseBlock(&_msg_queue_ptrs_allocator, q);
    if (res != Success) {
        printf("[ERROR] Failed to deallocate message queue ptr!\n");
        return -1;
    }
    return 0;
}

// void Resource_print(Resource* r)
void MsgQueuePtr_print(const MsgQueuePtr *q) {
    MsgQueue_print(q->msgQueuePtr);
}

// Resource* ResourceList_byId(ResourceList* l, int id)
MsgQueuePtr* MsgQueuePtrList_findByName(MsgQueuePtrList *l, const char *name) {
    if (name == NULL) return NULL;

    for (ListItem *current = l->first; current != NULL; current = current->next) {
        if (strcmp(name, ((MsgQueuePtr *)current)->msgQueuePtr->resource.name) == 0) {
            return (MsgQueuePtr *)current;
        }
    }
    return NULL;
}

// void ResourceList_print(ListHead* l)
void MsgQueuePtrList_print(MsgQueuePtrList *l) {
    printf("{\n");
    for (ListItem *currNode = l->first; currNode != NULL; currNode = currNode->next) {
        MsgQueuePtr *msgQueue = (MsgQueuePtr *)currNode;
        printf("\t");
        MsgQueuePtr_print(msgQueue);
        if (currNode->next != NULL)
            printf(",");
        printf("\n");
    }
    printf("}\n");
}

