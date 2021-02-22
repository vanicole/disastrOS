# disastrOS

E' stato implementato un sistema IPC (Inter-Process Communication) basato su code di messaggi in disastrOS in modo da consentire la comunicazione asincrona tra i processi.

************************************************************************************
Modifiche apportate alla struttura base di disastrOS:

1. E' stata creato un file **disastrOS_msg_queue.h** che contiene tutte le strutture dati necessarie per implementare una coda di messaggi: Text, Message, Subqueue, MsgQueue e MsgQueuePtr. Vengono allocate tramite pool allocator. Inoltre viene aggiunta una funzione er ricercare la coda tramite il nome: MsgQueuePtr * MsgQueuePtrList_findByName(MsgQueuePtrList *l, const char *name)_. Tutte le funzioni ivi dichiarate vengono implementate in **disastrOS_msg_queue.c**. 

2. Sono state implementate delle nuove syscalls per gestire la message queue: 
	- **disastrOS_msgQueueCreate.c**
	- **disastrOS_msgQueueOpen.c**
	- **disastrOS_msgQueueRead.c**
	- **disastrOS_msgQueueWrite.c**
	- **disastrOS_msgQueueClose.c**
	- **disastrOS_msgQueueUnlink.c**

3. Viene aggiunto il campo 'name' alla Resource, in **disastrOS_resource.h**

4. In **disastrOS_constants.h** vengono aggiunte una serie di costanti:
 - macro relative ai messaggi: MAX_TEXT_LEN, MAX_NUM_MESSAGES, MAX_NUM_MESSAGE_PER_MSG_QUEUE e MAX_NUM_PRIORITIES_. In particolare, modificando MAX_NUM_MESSAGES_PER_MSG_QUEUE_ è possibile testare l'errore causato dal tentativo di scrivere in una coda piena; invece MAX_NUM_PRIORITIES definisce il numero di sotto code che verranno create nella coda di messaggi.
- gli error code associati alle operazioni sulla coda di messaggi:   
DSOS_EMQ_READ, DSOS_EMQ_WRITE, DSOS_EMQ_CLOSE, DSOS_EMQ_UNLINK, DSOS_EMQ_NOFD e DSOS_EMQ_NOEXIST. 
- numeri di syscalls: DSOS_CALL_MQ_CREATE, DSOS_CALL_MQ_OPEN, DSOS_CALL_MQ_CLOSE, DSOS_CALL_MQ_UNLINK, DSOS_CALL_MQ_READ e DSOS_CALL_MQ_WRITE.
