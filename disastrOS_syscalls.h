#pragma once
#include <assert.h>
#include "disastrOS.h"
#include "disastrOS_globals.h"

void internal_preempt();
void internal_fork();
void internal_exit();
void internal_wait();
void internal_spawn();
void internal_shutdown();
void internal_schedule();
void internal_sleep();
void internal_openResource();
void internal_closeResource();
void internal_destroyResource();

/* SYSCALL MSG QUEUE */
void internal_msgQueueOpen();
void internal_msgQueueCreate();
void internal_msgQueueClose();
void internal_msgQueueUnlink();
void internal_msgQueueRead();
void internal_msgQueueWrite();

