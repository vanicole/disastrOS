# disastrOS

E' stato implementato un sistema IPC (Inter-Process Communication) basato su code di messaggi in disastrOS in modo da consentire la comunicazione asincrona tra i processi.

************************************************************************************
Modifiche apportate alla struttura base di disastrOS:

1. In **disastrOS_resource.h** viene aggiunto il campo 'name' alla struct Resource.
Di conseguenza viene modificato **disastrOS_open_resource.c** (riga 12), 

2. **disastrOS_msg_queue.h** contiene tutte le strutture dati necessarie per implementare una coda di messaggi: Text, Message, Subqueue, MsgQueue e MsgQueuePtr. Vengono allocate tramite pool allocator. 
Viene aggiunta una funzione per ricercare la coda tramite il nome: MsgQueuePtr * MsgQueuePtrList_findByName(MsgQueuePtrList *l, const char *name).
Tutte le funzioni ivi dichiarate vengono implementate in **disastrOS_msg_queue.c**. 


3. In **disastrOS_constants.h** vengono aggiunte una serie di costanti:
- macro relative ai messaggi: 
  - MAX_TEXT_LEN
  - MAX_NUM_MESSAGES
  - MAX_NUM_MESSAGE_PER_MSG_QUEUE 
  - MAX_NUM_PRIORITIES. 

Modificando MAX_NUM_MESSAGES_PER_MSG_QUEUE è possibile testare l'errore causato dal tentativo di scrivere in una coda piena; invece MAX_NUM_PRIORITIES definisce il numero di sotto code che verranno create nella coda di messaggi.

- gli error code associati alle operazioni sulla coda di messaggi:
  - DSOS_EMQ_READ 
  - DSOS_EMQ_WRITE
  - DSOS_EMQ_CLOSE
  - DSOS_EMQ_UNLINK
  - DSOS_EMQ_NOFD 
  - DSOS_EMQ_NOEXIST. 

- numeri di syscalls: 
  - DSOS_CALL_MQ_CREATE 
  - DSOS_CALL_MQ_OPEN 
  - DSOS_CALL_MQ_CLOSE
  - DSOS_CALL_MQ_UNLINK
  - DSOS_CALL_MQ_READ 
  - DSOS_CALL_MQ_WRITE.


4. In **disastrOS_syscalls.h** vengono dichiarate le implementazioni delle syscalls:
  - void internal_msgQueueOpen();
  - void internal_msgQueueCreate();
  - void internal_msgQueueClose();
  - void internal_msgQueueUnlink();
  - void internal_msgQueueRead();
  - void internal_msgQueueWrite();


5. Sono stati creati dei file C con l'implementazione delle nuove syscalls per gestire la message queue: 
	- **disastrOS_msgQueueCreate.c**
	- **disastrOS_msgQueueOpen.c**
	- **disastrOS_msgQueueRead.c**
	- **disastrOS_msgQueueWrite.c**
	- **disastrOS_msgQueueClose.c**
	- **disastrOS_msgQueueUnlink.c**


5. In **disastrOS.c** vengono installate le nuove syscalls: vengono aggiunte al vettore delle syscall del sistema operativo (syscall_vector)e viene specificato il numero di argomenti ed il loro ordine nel vettore degli argomenti (syscall_numarg); viene dichiarata, inizializzata e stampata una lista di code di messaggi (msg_queues_list); vengono inizializzate le nuove strutture associate alla coda (riga 152 - 156); vengono implementate le syscalls:
  - int disastrOS_msgQueueCreate(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_CREATE, name);
    }
  - int disastrOS_msgQueueOpen(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_OPEN, name);
    }
  - int disastrOS_msgQueueClose(int mqdes) {
      return disastrOS_syscall(DSOS_CALL_MQ_CLOSE, mqdes);
    }
  - int disastrOS_msgQueueUnlink(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_UNLINK, name);
    }
  - int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, unsigned msg_len) {
      return disastrOS_syscall(DSOS_CALL_MQ_READ, mqdes, msg_ptr, msg_len);
    }
  - int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned int priority) {
      return disastrOS_syscall(DSOS_CALL_MQ_WRITE, mqdes, msg_ptr, msg_len, priority);
    }


6. In **disastrOS.h** vengono dichiarate le funzioni chiamabili dal processo:
  - int disastrOS_msgQueueCreate(const char *name);
  - int disastrOS_msgQueueOpen(const char *name);
  - int disastrOS_msgQueueClose(int mqdes);
  - int disastrOS_msgQueueUnlink(const char *name);
  - int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, unsigned msg_len);
  - int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned priority);












 
