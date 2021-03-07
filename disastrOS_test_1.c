#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"

#define ITERATIONS 3
void producer() {
    char buf[256];
    printf("[PRODUCER pid = %d] Start!\n", disastrOS_getpid());

    int mqdes = disastrOS_msgQueueOpen("/mq");
    printf("[PRODUCER pid = %d] Msg queue with fd = %d opened. Descriptor mqdes allocated. \n", disastrOS_getpid(), mqdes);
    disastrOS_printStatus();

    printf("[PRODUCER pid = %d] preempt(): CPU to CONSUMER \n\n", disastrOS_getpid());
    printf("[CONSUMER] Running!\n");disastrOS_preempt();
    disastrOS_printStatus();

    unsigned int priority;

    for (unsigned int i = 0; i < ITERATIONS; ++i) {
        memset(buf, 0, 256);

        priority = rand() % 10; // MODIFICARE per impostare priorità precisa
        sprintf(buf, "Msg #%d", 1);
        printf("[PRODUCER pid = %d] Writing msg: '%s', priority: %u, size: %d\n\n",disastrOS_getpid(), buf, priority, (int)strlen(buf));
        disastrOS_msgQueueWrite(mqdes, buf, strlen(buf), priority);
        disastrOS_printStatus();

        memset(buf, 0, 256);

        priority = rand() % 10; // MODIFICARE per impostare priorità precisa
        sprintf(buf, "Msg #%d", 2);
        printf("[PRODUCER pid = %d] Writing msg: '%s', priority: %u, size: %d\n\n", disastrOS_getpid(), buf, priority, (int)strlen(buf));
        disastrOS_msgQueueWrite(mqdes, buf, strlen(buf), priority);
        disastrOS_printStatus();

        memset(buf, 0, 256);

        priority = rand() % 10; // MODIFICARE per impostare priorità precisa
        sprintf(buf, "Msg #%d", 3);
        printf("[PRODUCER pid = %d] Writing msg: '%s', priority: %u, size: %d \n\n", disastrOS_getpid(), buf, priority, (int)strlen(buf));
        disastrOS_msgQueueWrite(mqdes, buf, strlen(buf), priority);
        disastrOS_printStatus();


        printf("[PRODUCER pid = %d] preempt(): CPU to CONSUMER\n\n", disastrOS_getpid());
        printf("[CONSUMER] in running\n");
        disastrOS_preempt();

    }

    printf("[PRODUCER (pid = %d)] Deallocating subqueues and msg queue. Closing msg queue (fd = %d)\n", disastrOS_getpid(), mqdes);
    disastrOS_msgQueueClose(mqdes);
    //printf("[PRODUCER pid = %d] Msg queue (fd = %d) closed \n", disastrOS_getpid(), mqdes);
    disastrOS_printStatus();

    printf("[PRODUCER pid = %d] exit(0)\n", disastrOS_getpid());
    disastrOS_exit(0);

}

void consumer() {
    char buf[256];
    printf("[CONSUMER pid = %d] Start!\n", disastrOS_getpid());

    int mqdes = disastrOS_msgQueueOpen("/mq");
    printf("[CONSUMER pid = %d] Msg queue with fd = %d opened. Descriptor mqdes allocated. \n", disastrOS_getpid(), mqdes);

    disastrOS_printStatus();

    printf("[CONSUMER pid = %d] preempt: CPU to PRODUCER \n\n", disastrOS_getpid());
    printf("[PRODUCER] in running\n");
    disastrOS_preempt();
    disastrOS_printStatus();

    for (unsigned i = 0; i < ITERATIONS; ++i) {

        memset(buf, 0, 256);
        printf("[CONSUMER pid = %d] Reading first msg in the message queue\n", disastrOS_getpid());
        disastrOS_msgQueueRead(mqdes, buf, 256);
        disastrOS_printStatus();

        memset(buf, 0, 256);
        printf("[CONSUMER pid = %d] Readind second msg in the message queue\n", disastrOS_getpid());
        disastrOS_msgQueueRead(mqdes, buf, 256);
        disastrOS_printStatus();

        memset(buf, 0, 256);
        printf("[CONSUMER pid = %d] Reading third msg in the message queue\n", disastrOS_getpid());
        disastrOS_msgQueueRead(mqdes, buf, 256);
        disastrOS_printStatus();

        printf("[CONSUMER pid = %d] preempt(): CPU to PRODUCER\n", disastrOS_getpid());
        printf("[PRODUCER] in running\n");
        disastrOS_preempt();
    }

    printf("[CONSUMER pid = %d] Msg queue (fd = %d) closed\n", disastrOS_getpid(), mqdes);
    disastrOS_msgQueueClose(mqdes);
    disastrOS_printStatus();

    printf("[CONSUMER pid = %d] exit(0)\n", disastrOS_getpid());
    disastrOS_exit(0);
}

void childFunc(void *argsPtr) {
    printf("[childFunc pid = %d] Start! \n", disastrOS_getpid());

    disastrOS_spawn(consumer, NULL);
    printf("[childFunc pid = %d] Spawning consumer \n", disastrOS_getpid());
    disastrOS_printStatus();

    disastrOS_spawn(producer, NULL);
    printf("[childFunc pid = %d] Spawning producer \n", disastrOS_getpid());
    disastrOS_printStatus();

    printf("[childFunc pid = %d] Allocating subqueues and message queue\n\n", disastrOS_getpid());
    disastrOS_msgQueueCreate("/mq");
    disastrOS_printStatus();

    printf("[childFunc pid = %d] Goes to waiting list: waiting for termination of child process\n\n", disastrOS_getpid());

    disastrOS_wait(0, NULL);  // attende terminazione di uno qualsiasi dei figli
    printf("[childFunc pid = %d] wait(0, NULL) -> reads retvalue of terminated child\n", disastrOS_getpid());
    disastrOS_printStatus();

    printf("[childFunc pid = %d] Goes to waiting list: waiting for termination of next child process\n\n", disastrOS_getpid());

    disastrOS_wait(0, NULL);
    printf("[childFunc pid = %d] wait(0, NULL) -> reads the retvalue of terminated child\n", disastrOS_getpid());
    disastrOS_printStatus();

    // testing unlink

    //printf("[childFunc pid = %d] Trying to unlink the message queue\n", disastrOS_getpid());
    //disastrOS_msgQueueUnlink("/mq");
    //disastrOS_printStatus();

    printf("[childFunc pid = %d] exit(0)\n", disastrOS_getpid());
    disastrOS_exit(0);

}

void initFunc(void *argsPtr) {
    printf("[init (pid = %d)] Start! \n", disastrOS_getpid());

    printf("[init (pid = %d)] Spawning childFunc\n", disastrOS_getpid());
    disastrOS_spawn(childFunc, NULL);

    disastrOS_printStatus();

    printf("init goes on waiting list: waiting for termination of childFunc\n");
    disastrOS_wait(0, NULL);
    printf("[init] wait(0, NULL) --> reads the retvalue of terminated childFunc\n");

    disastrOS_printStatus();

    printf("[init] Shutdown!\n\n");
    disastrOS_shutdown();
}


int main(int argc, char** argv) {
    printf("\n*********************** IPC with MESSAGE QUEUE ***********************\n\n");
    char* logfilename = 0;
    if (argc > 1) {
        logfilename = argv[1];
    }
    // spawn an init process
    printf("Start! \n");
    disastrOS_start(initFunc, 0, logfilename);
    return 0;
}
