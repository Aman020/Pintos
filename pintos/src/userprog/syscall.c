#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/syscall-nr.h"
#include "userprog/syscall.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/malloc.h"

static void syscall_handler (struct intr_frame *);
static struct list file_list;

void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	list_init(&file_list);
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
		case SYS_OPEN:
						f->eax = open( (char *) (*((int*)f->esp + 1) ) );
						break;
		case SYS_READ:
						f->eax = read(*((int*)f->esp + 1), (void *) (*((int*)f->esp + 2) ), (unsigned) (*((int*)f->esp + 3)) );
						break;
		case SYS_FILESIZE:
						f->eax = filesize( *((int*)f->esp + 1) );
						break;
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
	//bool success = filesys_create(file, initial_size);
	//open(file);
	return filesys_create(file, initial_size);
}


int open(const char* file) {
	//return filesys_open(file);
	//struct file * file = filesys_open (file);
	struct file_descriptor *file_d = (struct file_descriptor *)malloc( sizeof(struct file_descriptor) );
	file_d->fd = list_size(&file_list) + 2;
	file_d->file = filesys_open (file);
	//printf("%s, %d", file, file_d->fd);
	//list_insert (struct list_elem *, struct list_elem *);
	list_push_front (&file_list, &file_d->felem);
	return file_d->fd;
}

int read(int fd, void* buffer, unsigned size) {
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd) {
			//printf("r - %d\n", fd);
			return file_read (f->file, buffer, size);
		}
	}
	return 0;
}

int filesize(int fd) {
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd) {
			//printf("r - %d\n", fd);
			return file_length (f->file);
		}
	}
	return 0;
}

int write (int fd , const void * buffer , unsigned size ) {
	if ( fd == STDOUT_FILENO ) {
		//printf("Need to write to console %s\n", (char *)buffer);
		putbuf(buffer, size);
		return size;
	}
	//printf("In write %d \n", fd);
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd) {
			return file_write (f->file, buffer, size);
		}
	}
	return 0;
	
}
