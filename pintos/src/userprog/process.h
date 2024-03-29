#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include <list.h>
#include "threads/synch.h"

void process_init(void);
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (int status);
void process_activate (void);
bool does_pid_waiting( tid_t child_tid );

/* A argv array. */
struct argv_array {
	void *addrs;						/* Addrs of the argument */
	struct list_elem arrelem;			/* List to store argv addresses. */
};

/* Wait for TID. */
struct waiting_tid {
	tid_t tid;							/* Child tid. */
	struct semaphore s;					/* Semaphore  for waiting list. */
	int status;							/* Status of child tid. */
	struct list_elem tidelem;			/* List to store argv addresses. */
};


#endif /* userprog/process.h */
