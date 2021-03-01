#pragma once

#define MAX_NUM_PROCESSES 1024
#define MAX_NUM_RESOURCES 1024
#define MAX_NUM_RESOURCES_PER_PROCESS 32
#define MAX_NUM_DESCRIPTORS_PER_PROCESS 32

// messages
#define MAX_TEXT_LEN                     255        // dimensione massima del messaggio (MAX_TEXT_LEN < 6: ERROR)
#define MAX_NUM_MESSAGES                1024        // numero massimo di messaggi che si possono scrivere (MAX_NUM_MESSAGES < 9: ERROR)
#define MAX_NUM_MESSAGES_PER_MSG_QUEUE    64        // numero massimo di messaggi nella message queue (modificare per testare coda piena, MAX_NUM_MESSAGES_PER_MSG_QUEUE < 3)
#define MAX_NUM_PRIORITIES                10        // numero di subqueue della msg queue (modificare per testare vari errori perchè la priorità è un intero tra 0 e 9)

#define STACK_SIZE        16384
// signals
#define MAX_SIGNALS   32
#define DSOS_SIGCHLD 0x1
#define DSOS_SIGHUP  0x2

// errors
#define DSOS_ESYSCALL_ARGUMENT_OUT_OF_BOUNDS -1
#define DSOS_ESYSCALL_NOT_IMPLEMENTED        -2
#define DSOS_ESYSCALL_OUT_OF_RANGE           -3
#define DSOS_EFORK      -4
#define DSOS_EWAIT      -5
#define DSOS_ESPAWN     -6
#define DSOS_ESLEEP     -7
#define DSOS_ERESOURCECREATE  -8
#define DSOS_ERESOURCEOPEN    -9
#define DSOS_ERESOURCENOEXCL -10
#define DSOS_ERESOURCENOFD   -11
#define DSOS_ERESOURCECLOSE  -12
#define DSOS_ERESOURCEINUSE  -13

#define DSOS_EMQ_NOFD    -14
#define DSOS_EMQ_NOEXIST -15

// errors msg queue
#define DSOS_EMQ_CREATE -16
#define DSOS_EMQ_READ   -17
#define DSOS_EMQ_WRITE  -18
#define DSOS_EMQ_CLOSE  -19
#define DSOS_EMQ_UNLINK -20

// syscall numbers
#define DSOS_MAX_SYSCALLS 32
#define DSOS_MAX_SYSCALLS_ARGS 8
#define DSOS_CALL_PREEMPT   1
#define DSOS_CALL_FORK      2
#define DSOS_CALL_WAIT      3
#define DSOS_CALL_EXIT      4
#define DSOS_CALL_SPAWN     5
#define DSOS_CALL_SLEEP     6
#define DSOS_CALL_OPEN_RESOURCE    7
#define DSOS_CALL_CLOSE_RESOURCE   8
#define DSOS_CALL_DESTROY_RESOURCE 9
#define DSOS_CALL_SHUTDOWN        10

// syscall numbers msg queue
#define DSOS_CALL_MQ_CREATE  11
#define DSOS_CALL_MQ_OPEN    12
#define DSOS_CALL_MQ_CLOSE   13
#define DSOS_CALL_MQ_UNLINK  14
#define DSOS_CALL_MQ_READ    15
#define DSOS_CALL_MQ_WRITE   16

//resources
#define DSOS_CREATE 0x1
#define DSOS_READ   0x2
#define DSOS_WRITE  0x3
#define DSOS_EXCL   0x4

// scheduling
#define ALPHA 0.5f
#define INTERVAL 100 // milliseconds for timer tick
