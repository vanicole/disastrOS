#pragma once
#include "disastrOS_pcb.h"
#include "disastrOS_msg_queue.h"
#include "linked_list.h"

#ifdef _DISASTROS_DEBUG_
#include <stdio.h>

#define disastrOS_debug(...) printf(__VA_ARGS__)

#else //_DISASTROS_DEBUG_

#define disastrOS_debug(...) ;

#endif //_DISASTROS_DEBUG_

// initializes the structures and spawns a fake init process
void disastrOS_start(void (*f)(void*), void* args, char* logfile);

// generic syscall
int disastrOS_syscall(int syscall_num, ...);

// classical process control
int disastrOS_getpid(); // this should be a syscall, but we have no memory separation, so we return just the running pid
int disastrOS_fork();
void disastrOS_exit(int exit_value);
int disastrOS_wait(int pid, int* retval);
void disastrOS_preempt();
void disastrOS_spawn(void (*f)(void*), void* args );
void disastrOS_shutdown();

// timers
void disastrOS_sleep(int);

// resources (files)
int disastrOS_openResource(const char *namePtr, int resource_id, int type, int mode);
int disastrOS_closeResource(int fd);
int disastrOS_destroyResource(int resource_id);

//message queues
int disastrOS_msgQueueCreate(const char *name);
int disastrOS_msgQueueOpen(const char *name);
int disastrOS_msgQueueClose(int mqdes);
int disastrOS_msgQueueUnlink(const char *name);
int disastrOS_msgQueueRead(int mqdes, char *msg_ptr, unsigned msg_len);
int disastrOS_msgQueueWrite(int mqdes, const char *msg_ptr, unsigned msg_len, unsigned priority);
int disastrOS_msgQueueWait(int mqdes);

// debug function, prints the state of the internal system
void disastrOS_printStatus();
