#pragma once

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "pool_allocator.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"

// Funzioni associate a Text del msg
void Text_init();
char* Text_alloc();
int Text_free(char *s);

// Definisce messaggio
typedef struct Message {
    ListItem list;
    char *msg_ptr; // text ptr
    unsigned msg_len; // length
} Message;

void Message_init();
Message* Message_alloc(const char* msg, unsigned size);
int Message_free(Message* msg);
void Message_print(const Message* msg);

// Definisce lista di messaggi
typedef ListHead MessageList;
void MessageList_print(MessageList *list);



// Definisce sottocoda di messaggi con priorità fissata
typedef struct Subqueue {
    ListItem list;
    MessageList messages;
    unsigned priority;
} Subqueue;

void Subqueue_init();
Subqueue * Subqueue_alloc(unsigned priority);
int Subqueue_free(Subqueue *msg);
void Subqueue_print(const Subqueue *msg);



// definisce coda di messaggi
typedef struct MsgQueue {
    Resource resource;
    Subqueue *subqueues[MAX_NUM_PRIORITIES];
    PCB* pcb;                                       // identifica processo running che alloca msg queue
    int size;                                       // numero di messaggi che compongono la coda
    ListHead waiting_descriptors;
} MsgQueue;

void MsgQueue_init();
MsgQueue* MsgQueue_alloc(const char *name, int id, PCB *pcb);
int MsgQueue_free(MsgQueue *q);
void MsgQueue_print(const MsgQueue *q);



// Definisce un riferimento alla coda di msg
typedef struct MsgQueuePtr {
    ListItem list;
    MsgQueue *msgQueuePtr;
} MsgQueuePtr;

void MsgQueuePtr_init();
MsgQueuePtr* MsgQueuePtr_alloc(MsgQueue *q);
int MsgQueuePtr_free(MsgQueuePtr *q);
void MsgQueuePtr_print(const MsgQueuePtr *q);

// Definisce lista di riferimenti alle code di msg
typedef ListHead MsgQueuePtrList;

// Ricerca per nome
MsgQueuePtr * MsgQueuePtrList_findByName(MsgQueuePtrList *l, const char *name);
void MsgQueuePtrList_print(MsgQueuePtrList *l);
