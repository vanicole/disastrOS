# disastrOS

E' stato implementato un sistema IPC (Inter-Process Communication) basato su code di messaggi in disastrOS in modo da consentire la comunicazione asincrona tra i processi.

************************************************************************************
Modifiche apportate alla struttura base di disastrOS:

1. E' stata creato un file disastrOS_msg_queue.h che contiene tutte le strutture dati necessarie per implementare una coda di messaggi: Text, Message, Subqueue, MsgQueue e MsgQueuePtr.

2. Sono state inserite delle nuove syscalls per gestire la message queue: 
	- disastrOS_msgQueueCreate.c
	- disastrOS_msgQueueOpen.c
	- disastrOS_msgQueueRead.c
	- disastrOS_msgQueueWrite.c
	- disastrOS_msgQueueClose.c
	- disastrOS_msgQueueUnlink.c

3. Viene aggiunto il campo 'name' alla Resource, in disastrOS_resource.h

