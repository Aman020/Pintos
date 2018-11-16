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
tid_t exec (const char * cmd_line );
int wait (tid_t pid );
void seek (int fd , unsigned position );
void close (int fd );

void sys_deny_write(char *);
void sys_allow_write(char *);

struct file;

/* File descriptor and struct file */
struct file_descriptor {
	int fd;								/* File descriptor */
	struct file *file;					/* corresponding struct file pointer */
	const char *name;					/* File name */
	struct list_elem felem;				/* List to store the structure. */
};

#endif /* userprog/syscall.h */
