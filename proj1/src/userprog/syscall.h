#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "filesys/off_t.h"
#include <stdio.h>

int fibonacci(int);
int max_of_four_int(int, int, int, int);
void syscall_init (void);
/* proj2 */
struct lock thread_lock;

struct file
{
	struct inode *inode;
	off_t pos;
	bool deny_write;
};
#endif /* userprog/syscall.h */
