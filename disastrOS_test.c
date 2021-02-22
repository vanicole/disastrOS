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
    int mq_fd = disastrOS_msgQueueOpen("/mq");

    printf("[PRODUCER pid = %d] Msg queue with fd = %d opened \n", disastrOS_getpid(), mq_fd);
    disastrOS_printStatus();
    printf("[PRODUCER pid = %d] preempt(): CPU to CONSUMER \n\n", disastrOS_getpid());
    printf("[CONSUMER] Running!\n");disastrOS_preempt();
    disastrOS_printStatus();

    unsigned priority;

    for (unsigned i = 0; i < ITERATIONS; ++i) {
        memset(buf, 0, 256);

        priority = rand() % 10; // MODIFICARE per impostare priorità precisa
        sprintf(buf, "Msg #%u", 1);
        printf("[PRODUCER pid = %d] Write msg: '%s', priority: %u\n",disastrOS_getpid(), buf, priority);
        disastrOS_msgQueueWrite(mq_fd, buf, strlen(buf), priority);
        disastrOS_printStatus();

        memset(buf, 0, 256);

        priority = rand() % 10; // MODIFICARE per impostare priorità precisa
        sprintf(buf, "Msg #%u", 2);
        printf("[PRODUCER pid = %d] Write msg: '%s', priority: %u\n", disastrOS_getpid(), buf, priority);
        disastrOS_msgQueueWrite(mq_fd, buf, strlen(buf), priority);
        disastrOS_printStatus();

        memset(buf, 0, 256);

        priority = rand() % 10; // MODIFICARE per impostare priorità precisa
        sprintf(buf, "Msg #%u", 3);
        printf("[PRODUCER pid = %d] Write msg: '%s', priority: %u\n", disastrOS_getpid(), buf, priority);
        disastrOS_msgQueueWrite(mq_fd, buf, strlen(buf), priority);
        disastrOS_printStatus();


        printf("[PRODUCER pid = %d] preempt(): CPU to CONSUMER\n\n", disastrOS_getpid());
        printf("[CONSUMER] in running\n");
        disastrOS_preempt();

    }

    disastrOS_msgQueueClose(mq_fd);
    printf("[PRODUCER pid = %d] Msg queue (fd = %d) closed \n", disastrOS_getpid(), mq_fd);
    disastrOS_printStatus();

    printf("[PRODUCER pid = %d] exit(0)\n", disastrOS_getpid());
    disastrOS_exit(0);

}

void consumer() {
    char buf[256];
    printf("[CONSUMER pid = %d] Start!\n", disastrOS_getpid());

    int mq_fd = disastrOS_msgQueueOpen("/mq");
    printf("[CONSUMER pid = %d] Msg queue (fd = %d) opened \n", disastrOS_getpid(), mq_fd);
    disastrOS_printStatus();
    printf("[CONSUMER pid = %d] preempt: CPU to PRODUCER \n\n", disastrOS_getpid());
    printf("[PRODUCER] in running\n");
    disastrOS_preempt();
    disastrOS_printStatus();

    for (unsigned i = 0; i < ITERATIONS; ++i) {

        memset(buf, 0, 256);
        printf("[CONSUMER pid = %d] Reading first msg\n", disastrOS_getpid());
        disastrOS_msgQueueRead(mq_fd, buf, 256);
        disastrOS_printStatus();

        memset(buf, 0, 256);
        printf("[CONSUMER pid = %d] Readind second msg\n", disastrOS_getpid());
        disastrOS_msgQueueRead(mq_fd, buf, 256);
        disastrOS_printStatus();

        memset(buf, 0, 256);
        printf("[CONSUMER pid = %d] Reading third msg\n", disastrOS_getpid());
        disastrOS_msgQueueRead(mq_fd, buf, 256);
        disastrOS_printStatus();

        printf("[CONSUMER pid = %d] preempt(): CPU to PRODUCER\n", disastrOS_getpid());
        printf("[PRODUCER] in running\n");
        disastrOS_preempt();
    }

    printf("[CONSUMER pid = %d] Msg queue (fd = %d) closed\n", disastrOS_getpid(), mq_fd);
    disastrOS_msgQueueClose(mq_fd);
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

    disastrOS_msgQueueCreate("/mq");
    printf("[childFunc pid = %d] Msg queue created \n", disastrOS_getpid());
    disastrOS_printStatus();

    printf("[childFunc pid = %d] go to waiting list: waiting termination of child process\n", disastrOS_getpid());

    disastrOS_wait(0, NULL);  // attende terminazione di uno qualsiasi dei figli
    printf("[childFunc pid = %d] wait(0) -> read retvalue of terminated child\n", disastrOS_getpid());
    disastrOS_printStatus();

    printf("[childFunc pid = %d] go to waiting list: Waiting termination of next child process\n", disastrOS_getpid());

    disastrOS_wait(0, NULL);
    printf("[childFunc pid = %d] wait(0) -> read retvalue of terminated child\n", disastrOS_getpid());
    disastrOS_printStatus();

    disastrOS_msgQueueUnlink("/mq");
    printf("[childFunc pid = %d] Unlinked msg queue\n", disastrOS_getpid());
    disastrOS_printStatus();

    printf("[childFunc pid = %d] exit(0)\n", disastrOS_getpid());
    disastrOS_exit(0);

}

void initFunc(void *argsPtr) {
    printf("[init (pid = %d)] Start! \n", disastrOS_getpid());

    printf("[init (pid = %d)] Spawning childFunc\n", disastrOS_getpid());
    disastrOS_spawn(childFunc, NULL);

    disastrOS_printStatus();

    printf("init put in waiting list: waiting termination of childFunc\n");
    disastrOS_wait(0, NULL);
    printf("[init] wait(0) --> read retvalue of childFunc\n");

    disastrOS_printStatus();

    printf("[init] Shutdown!\n\n");
    disastrOS_shutdown();
}

int main(int argc, char** argv) {
    printf("\n*********************** IPC with MESSAGE QUEUE ***********************\n\n");
    printf("Start!\n");
    disastrOS_start(initFunc, 0, NULL);
    return 0;
}
