#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void halt (void);
int write (int fd , const void * buffer , unsigned size );
void exit(int status);
bool create (const char * file , unsigned initial_size);
int open(const char* file);
int read(int fd, void* buffer, unsigned size);
int filesize(int fd);

/* File descriptor and struct file */
struct file_descriptor {
	int fd;								/* File descriptor */
	struct file * file;						/* corresponding struct file pointer */
	struct list_elem felem;				/* List to store the structure. */
};

#endif /* userprog/syscall.h */
