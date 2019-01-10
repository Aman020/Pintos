#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "lib/syscall-nr.h"
#include "userprog/syscall.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/malloc.h"
#include "userprog/process.h"
#include "filesys/inode.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

static struct list file_list;

struct lock l;

void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	list_init(&file_list);
	init_file();
	lock_init(&l);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	//printf("Inside handler : \n");
	if ( pagedir_get_page(thread_current()->pagedir, f->esp ) == NULL ) {
		exit(-1);
		return;
	}
	//printf("Running handler : \n");
  
  int sys_code = *(int*)f->esp;
	//printf("SYS : %d \n", sys_code);
	switch(sys_code) {
		case SYS_WRITE:
						lock_acquire(&l);
						if ( pagedir_get_page(thread_current()->pagedir, (const void *) (*((int*)f->esp + 2) )  ) == NULL ) {
							lock_release(&l);
							exit(-1);
							break;
						}
						lock_release(&l);
						f->eax = write(*((int*)f->esp + 1), (void *) (*((int*)f->esp + 2) ), (unsigned) (*((int*)f->esp + 3)) );
						break;
		case SYS_EXIT:
						lock_acquire(&l);
						if ( !is_user_vaddr( (const void *) (*((int*)f->esp + 1) )  ) ) {
							lock_release(&l);
							exit(-1);
							break;
						}
						lock_release(&l);
						exit(*((int*)f->esp + 1));
						break;
		case SYS_CREATE:
						lock_acquire(&l);
						if ( pagedir_get_page(thread_current()->pagedir, (const void *) (*((int*)f->esp + 1) )  ) == NULL ) {
							lock_release(&l);
							exit(-1);
							break;
						}
						lock_release(&l);
						f->eax = create((char *) (*((int*)f->esp + 1) ), (unsigned) (*((int*)f->esp + 2) ) );
						break;
		case SYS_OPEN:
						lock_acquire(&l);
						if ( pagedir_get_page(thread_current()->pagedir, (const void *) (*((int*)f->esp + 1) )  ) == NULL ) {
							lock_release(&l);
							exit(-1);
							break;
						}
						lock_release(&l);
						
						f->eax = open( (char *) (*((int*)f->esp + 1) ) );
						break;
		case SYS_READ:
						f->eax = read(*((int*)f->esp + 1), (void *) (*((int*)f->esp + 2) ), (unsigned) (*((int*)f->esp + 3)) );
						break;
		case SYS_FILESIZE:
						f->eax = filesize( *((int*)f->esp + 1) );
						break;
		case SYS_EXEC:
						lock_acquire(&l);
						if ( pagedir_get_page(thread_current()->pagedir, (const void *) (*((int*)f->esp + 1) )  ) == NULL ) {
							lock_release(&l);
							exit(-1);
							break;
						}
						lock_release(&l);
						f->eax = exec( (char * ) ( *((int*)f->esp + 1) ) );
						break;
		case SYS_WAIT:
					
						/*
						if ( pagedir_get_page(thread_current()->pagedir, (const void *) (*((int*)f->esp + 1) )  ) == NULL ) {
							exit(-1);
							break;
						}
						*/
						f->eax = wait( (tid_t ) ( *((int*)f->esp + 1) ) );
						break;
		case SYS_SEEK:
						seek( *((int*)f->esp + 1), (unsigned) (*((int*)f->esp + 2)) );
						break;
		case SYS_CLOSE:
						close( *((int*)f->esp + 1) );
						break;
		case SYS_HALT:
						halt();
						break;
		default:
						//exit(-1);
						break;
			//printf("default : %d \n", sys_code);
	}
  
  //printf ("system call! %d == %d\n", sys_code, SYS_WRITE);
  
  //thread_exit ();
}

void halt (void) {
	shutdown_power_off();
}

void exit(int status) {
	//if ( status != 0 )
	//	status = -1;
	
	printf("%s: exit(%d)\n", thread_current()->name, status);
	//process_exit();
	thread_exit(status);
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
	//printf("%d \n", file == NULL);
	if ( file == NULL ) {
		exit(-1);
		return false;
	}
	/*
	if( !is_kernel_vaddr(file) ) {
		exit(-1);
		return false;
	}
	*/
	lock_acquire(&l);
	bool t = filesys_create(file, initial_size);
	lock_release(&l);
	return t;
}


int open(const char* file) {
	//return filesys_open(file);
	//struct file * file = filesys_open (file);
	
	/*
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->name == file) {
			printf("r - %d -- SHIVRAJ \n", f->fd);
			return f->fd;
		}
	}
	*/
	
	if( !is_user_vaddr((void *)file) ) {
		exit(-1);
		return false;
	}
	
	if ( file == NULL ) {
		exit(-1);
		return -1;
	}
	//printf("Opeining file %s %d \n ", file);
	
	if(strcmp(file, "") == 0)
		return -1;
	
	struct file_descriptor *file_d = (struct file_descriptor *)malloc( sizeof(struct file_descriptor) );
	file_d->fd = list_size(&file_list) + 2;
	lock_acquire(&l);
	file_d->file = filesys_open (file);
	lock_release(&l);
	if( file_d->file == NULL ) {
		free(file_d);
		return -1;
	}
	file_d->owner = thread_current()->tid;
	//printf("Opeining file after %d \n", file_d->file->inode->sector);
	//file_d->name = file;
	//if( file_d->file != NULL)
	//list_insert (struct list_elem *, struct list_elem *);
	list_push_front (&file_list, &file_d->felem);
	return file_d->fd;
}

int read(int fd, void* buffer, unsigned size) {
	if (!is_user_vaddr (buffer + size)) {
		exit(-1);
		return -1;
	}
	struct list_elem *e;
	int t;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd) {
			//printf("r - %d\n", fd);
			lock_acquire(&l);
			t = file_read (f->file, buffer, size);
			lock_release(&l);
			return t;
		}
	}
	return 0;
}

int filesize(int fd) {
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		int t;
		if(f->fd == fd) {
			lock_acquire(&l);
			t = file_length (f->file);
			lock_release(&l);
			return t;
		}
	}
	return 0;
}

int wait (tid_t pid ) {
	
	//printf("wait %d \n", pid);
	
	if ( !does_pid_exist ( pid) ) {
		if ( !does_pid_waiting(pid) ) {				
			//printf("calling -1 %d \n", pid);
			exit(-1);
			return -1;
		}
	}
	
	//lock_acquire(&l);
	//
	int t = process_wait(pid);
	//lock_release(&l);
	//printf("done waiting %d \n", pid);
	return t;
}

tid_t exec (const char * cmd_line ) {
	//return execv(cmd_line);
	//printf("%d %d\n", !is_user_vaddr (cmd_line), is_kernel_vaddr(cmd_line) );
	
	/*
	if (!is_kernel_vaddr(cmd_line) ) {
		exit(-1);
		return -1;
	}
	*/
	
	lock_acquire(&l);
	//printf("Exec %s \n", cmd_line);
	tid_t t = process_execute(cmd_line);
	
	lock_release(&l);
	//printf("Done Exec %s \n", cmd_line);
	return t;
}

int write (int fd , const void * buffer , unsigned size ) {
	
	if ( fd == STDOUT_FILENO ) {
		//printf("Need to write to console %s\n", (char *)buffer);
		putbuf(buffer, size);
		return size;
	}
	//printf("In write %d \n", fd);
	struct list_elem *e;
	int t;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd) {
			lock_acquire(&l);
			t = file_write (f->file, buffer, size);
			lock_release(&l);
			return t;
		}
	}
	return 0;
}

void seek (int fd , unsigned position ) {
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd) {
			lock_acquire(&l);
			file_seek (f->file, position);
			lock_release(&l);
		}
	}
}

void close (int fd ) {
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(f->fd == fd && f->owner == thread_current()->tid ) {
			//printf("FILE -----------+++++++++++++++++ Closing the file %s \n", f->name);
			lock_acquire(&l);
			file_close (f->file);
			lock_release(&l);
			return;
		}
	}
	exit(-1);
}

//void sys_deny_write(char *token)  {
void sys_deny_write(char *token, tid_t tid)  {
	/*
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		if(strcmp(f->name, token) == 0) {
			//printf(" $$$$$$$$$$$$$$$$ Deny write %s %d %d\n", token, f->file->inode->sector, f->file->inode->deny_write_cnt);
			file_deny_write(f->file);
			//return;
		}
	}
	*/
	
	struct file_descriptor *file_d = (struct file_descriptor *)malloc( sizeof(struct file_descriptor) );
	file_d->fd = list_size(&file_list) + 2;
	file_d->file = filesys_open (token);
	//file_d->name = token;
	file_d->tid = tid;
	//printf("Deny write %s %p\n", token, file_d->file);
	file_deny_write(file_d->file);
	
	list_push_front (&file_list, &file_d->felem);
}

//void sys_allow_write(char *name) {
void sys_allow_write(tid_t tid) {
	//printf("In sys allow %s \n", name);
	struct list_elem *e;
	for (e = list_begin (&file_list); e != list_end (&file_list);	e = list_next (e)) {
		struct file_descriptor *f = list_entry (e, struct file_descriptor, felem);
		//printf(" %s == %s is %d \n ", f->name, name, f->name == name );
		//printf("%p \n", f->file);
		//if(strcmp(f->name, name) == 0) {
		if(f->tid == tid ) {
			//f->file->inode->sector = 107;
			//printf("rrrr $$$$$$$$$$$$ - %s, %d %d %d %d\n", name, f->file->deny_write, f->file->inode->sector, f->file->inode->open_cnt, f->file->inode->deny_write_cnt);
			//lock_acquire(&l);
			file_allow_write (f->file);
			//lock_release(&l);
			//return;
		}
	}
}

/*
void lock_acquire_sys(){
	lock_acquire(&l);
}
*/
/*
void lock_release_sys() {
	lock_release(&l);
}
*/
