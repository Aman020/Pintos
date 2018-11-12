#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/syscall-nr.h"
#include "userprog/syscall.h"
#include "filesys/filesys.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  
  int sys_code = *(int*)f->esp;
  
	switch(sys_code) {
		case SYS_WRITE:
						f->eax = write(*((int*)f->esp + 1), (void *) (*((int*)f->esp + 2) ), (unsigned) (*((int*)f->esp + 3)) );
						break;
		case SYS_EXIT:
						exit(*((int*)f->esp + 1));
						break;
		case SYS_CREATE:
						f->eax = create((char *) (*((int*)f->esp + 1) ), (unsigned) (*((int*)f->esp + 2) ) );
						break;
		//case SYS_OPEN:
		//				f->eax = open( (char *) (*((int*)f->esp + 1) ) );
		//				break;
		default:
			break;
			//printf("default : %d \n", sys_code);
	}
  
  //printf ("system call! %d == %d\n", sys_code, SYS_WRITE);
  
  //thread_exit ();
}

void halt (void) {
	
}

void exit(int status) {
	//if ( status != 0 )
	//	status = -1;
	printf("%s: exit(%d)\n", thread_current()->name, status);
	thread_exit();
}

bool create (const char * file , unsigned initial_size UNUSED) {
	//printf("create(%s): 0", file);
	/*
	FILE *fp = fopen(file, "w+");
	if ( fp ) {
		fclose(fp);
		return true;
	}
	*/
	return filesys_create(file, initial_size);
}

/*
int open(const char* file) {
	return 0;
}*/

int write (int fd , const void * buffer , unsigned size ) {
	if ( fd == STDOUT_FILENO ) {
		//printf("Need to write to console %s\n", (char *)buffer);
		putbuf(buffer, size);
		return size;
	}
	return 0;
}
