# disastrOS

### E' stato implementato un sistema IPC (Inter-Process Communication) basato su code di messaggi in disastrOS in modo da consentire la comunicazione asincrona tra i processi.

************************************************************************************
************************************************************************************

#### Modifiche apportate alla struttura base di disastrOS:

1. In [**disastrOS_resource.h**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_resource.h) viene aggiunto il campo 'name' alla struct Resource. Di conseguenza viene modificato il file [**disastrOS_open_resource.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_open_resource.c).


2. [**disastrOS_msg_queue.h**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msg_queue.h) contiene tutte le strutture dati necessarie per implementare una coda di messaggi: Text, Message, Subqueue, MsgQueue e MsgQueuePtr. Vengono allocate tramite SLAB allocator.
Viene aggiunta una funzione per ricercare la coda tramite il nome: `MsgQueuePtr * MsgQueuePtrList_findByName(MsgQueuePtrList *l, const char *name)`, che verrà usata in [**disastrOS_msgQueueUnlink.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueUnlink.c).
Tutte le funzioni ivi dichiarate vengono implementate in [**disastrOS_msg_queue.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msg_queue.c). Qua viene inserita la costante DSOS_MSG_QUEUE_RESOURCE per impostare il tipo di risorsa.


3. In [**disastrOS_constants.h**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_constants.h) vengono aggiunte una serie di costanti:
  - macro relative ai messaggi:
```c
	MAX_TEXT_LEN
  	MAX_NUM_MESSAGES
  	MAX_NUM_MESSAGE_PER_MSG_QUEUE
  	MAX_NUM_PRIORITIES
```

In particolare, MAX_NUM_MESSAGES_PER_MSG_QUEUE rappresenta il numero massimo di messaggi contenuti nella message queue: impostando un valore inferiore a 3 è possibile testare l'errore causato dal tentativo di scrivere in una coda piena (vedi riga 29 in [disastrOS_msgQueueWrite.c](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueWrite.c)); MAX_NUM_PRIORITIES indica il numero di sotto-code che verranno create nella coda di messaggi; MAX_TEXT_LEN rappresenta la dimensione del messaggio scritto: settando un valore inferiore a 6 è possibile testare gli errori di scrittura (vedi riga 35 in [disastrOS_msgQueueWrite.c](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueWrite.c)); infine MAX_NUM_MESSAGES indica il numero massimo di messaggi che si possono scrivere: se il valore è inferiore a 9 causerà segmentation fault nel momento in cui cerca di allocare l'ultimo messaggio.


  - gli error code associati alle operazioni sulla coda di messaggi:
```c
	DSOS_EMQ_CREATE
  	DSOS_EMQ_READ
  	DSOS_EMQ_WRITE
  	DSOS_EMQ_CLOSE
  	DSOS_EMQ_UNLINK
  	DSOS_EMQ_NOFD
  	DSOS_EMQ_NOEXIST
```
  - numeri di syscalls:
```c
  	DSOS_CALL_MQ_CREATE
  	DSOS_CALL_MQ_OPEN
  	DSOS_CALL_MQ_CLOSE
  	DSOS_CALL_MQ_UNLINK
  	DSOS_CALL_MQ_READ
  	DSOS_CALL_MQ_WRITE
```

4. In [**disastrOS_syscalls.h**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_syscalls.h) vengono dichiarate le implementazioni delle syscalls:
```c
   	void internal_msgQueueCreate();
   	void internal_msgQueueOpen();
   	void internal_msgQueueClose();
   	void internal_msgQueueUnlink();
   	void internal_msgQueueRead();
   	void internal_msgQueueWrite();
```

5. Sono stati creati dei file C con l'implementazione delle nuove syscalls per gestire la message queue:
   - [**disastrOS_msgQueueCreate.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueCreate.c)
   - [**disastrOS_msgQueueOpen.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueOpen.c)
   - [**disastrOS_msgQueueRead.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueRead.c)
   - [**disastrOS_msgQueueWrite.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueWrite.c)
   - [**disastrOS_msgQueueClose.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueClose.c)
   - [**disastrOS_msgQueueUnlink.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueUnlink.c)


5. In [**disastrOS.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS.c) vengono installate le nuove syscalls: vengono aggiunte al vettore delle syscall del sistema operativo (syscall_vector) e viene specificato il numero di argomenti ed il loro ordine nel vettore degli argomenti (syscall_numarg); viene dichiarata, inizializzata e stampata una lista di code di messaggi (msg_queues_list); vengono inizializzate le nuove strutture associate alla coda (riga 152 - 156). Vengono implementate le syscalls:

```c
   int disastrOS_msgQueueCreate(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_CREATE, name);
   }
   int disastrOS_msgQueueOpen(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_OPEN, name);
   }
   int disastrOS_msgQueueClose(int mqdes) {
      return disastrOS_syscall(DSOS_CALL_MQ_CLOSE, mqdes);
   }
   int disastrOS_msgQueueUnlink(const char *name) {
      return disastrOS_syscall(DSOS_CALL_MQ_UNLINK, name);
   }
   int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, unsigned int msg_len) {
      return disastrOS_syscall(DSOS_CALL_MQ_READ, mqdes, msg_ptr, msg_len);
   }
   int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned int priority) {
      return disastrOS_syscall(DSOS_CALL_MQ_WRITE, mqdes, msg_ptr, msg_len, priority);
   }
```

6. In [**disastrOS.h**](https://github.com/vanicole/disastrOS/blob/main/disastrOS.h) vengono dichiarate le funzioni chiamabili dal processo:

   - **int disastrOS_msgQueueCreate(const char *name)**: prende in input il nome della coda di messaggi da creare (sarà del tipo _/name_) e alloca le subqueue che compongono la coda e la coda stessa. Ritorna 0 in caso di successo, DSOS_EMQ_CREATE altrimenti.

   - **int disastrOS_msgQueueOpen(const char *name)**: prende in input il nome della coda da aprire e alloca il descrittore associato alla coda di messaggi aperta. Ritorna il file descriptor associato al descrittore del processo in caso di successo, altrimenti un codice di errore (DSOS_EMQ_NOFD e DSOS_EMQ_NOEXIST).

   - **int disastrOS_msgQueueClose(int mqdes)**: prende in input il descrittore mqdes associato alla coda di messaggi da chiudere. Ritorna 0 in caso di successo, DSOS_EMQ_CLOSE altrimenti. Se tutte le risorse hanno chiuso il descrittore associato alla coda, allora viene eseguita [**disastrOS_msgQueueUnlink.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msgQueueUnlink.c)

   - **int disastrOS_msgQueueUnlink(const char *name)**: prende in input il nome della coda di messaggi da rimuovere immediatamente. La coda verrà distrutta una volta che tutti i processi che l'hanno aperta chiudono i propri descrittori associati alla coda. Ritorna 0 in caso di successo, DSOS_EMQ_UNLINK altrimenti.

   - **int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, int unsigned msg_len)**: se ci sono messaggi nella coda identificata da mqdes, rimuove quello con priorità più alta e lo inserisce nel buffer a cui punta msg_ptr tramite la funzione `char* strcpy(char *dest, const char *src)`. Viene inoltre specificata la dimensione del buffer puntato da msg_ptr (256) per poter verificare che la dimensione del messaggio da scrivere sia entro questo valore. Questa funzione ritorna la lunghezza del messaggio letto in caso di successo, altrimenti DSOS_EMQ_READ.

   - **int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned int priority)**: prende in input il messaggio, puntato da msg_ptr, da inserire nella coda di messaggi identificata dal descrittore mqdes; viene specificata la lunghezza del messaggio puntato da msg_ptr e la priorità assegnata al messaggio. Ritorna la lunghezza del messaggio in caso di successo, DSOS_EMQ_WRITE altrimenti.
In particolare, viene allocato il messaggio tramite Message_alloc (vedi [*disastrOS_msgQueue.c*](https://github.com/vanicole/disastrOS/blob/main/disastrOS_msg_queue.c)) e inserito nella lista di messaggi associata alla sottocoda con quella priorità.

7. in [**disastrOS_test.c**](https://github.com/vanicole/disastrOS/blob/main/disastrOS_test.c) viene implementata la comunicazione tra un producer ed un consumer.
E' definita una costante ITERATIONS per settare il numero di iterazioni.

****************************************************************************************************************
****************************************************************************************************************
## Esecuzione

Per testare il funzionamento eseguire i seguenti passi:
* `git clone https://github.com/vanicole/disastrOS.git`
* `cd disastrOS`
* `make`
* `./disastrOS_test`
















