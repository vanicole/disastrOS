CC=gcc
CCOPTS=--std=gnu99 -Wall
AR=ar

HEADERS=disastrOS_msg_queue.h\
	disastrOS.h\
	disastrOS_constants.h\
	disastrOS_descriptor.h\
	disastrOS_globals.h\
	disastrOS_pcb.h\
	disastrOS_resource.h\
	disastrOS_syscalls.h\
	disastrOS_timer.h\
	linked_list.h\
	pool_allocator.h\

OBJS=pool_allocator.o\
     linked_list.o\
     disastrOS_timer.o\
     disastrOS_pcb.o\
     disastrOS_resource.o\
     disastrOS_descriptor.o\
     disastrOS.o\
     disastrOS_fork.o\
     disastrOS_wait.o\
     disastrOS_spawn.o\
     disastrOS_exit.o\
     disastrOS_shutdown.o\
     disastrOS_schedule.o\
     disastrOS_preempt.o\
     disastrOS_sleep.o\
     disastrOS_open_resource.o\
     disastrOS_close_resource.o\
     disastrOS_destroy_resource.o\
     disastrOS_msgQueueOpen.o\
     disastrOS_msgQueueClose.o\
     disastrOS_msgQueueCreate.o\
     disastrOS_msgQueueRead.o\
     disastrOS_msgQueueWrite.o\
     disastrOS_msgQueueUnlink.o\
     disastrOS_msgQueueWait.o\
     disastrOS_msg_queue.o\

LIBS=libdisastrOS.a

BINS=disastrOS_test

#disastros_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libdisastrOS.a: $(OBJS) $(HEADERS)
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

disastrOS_test:		disastrOS_test.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
