#include "userprog/syscall.h"
#include <stdio.h>
#include <stdlib.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);

int fibonacci(int n){
	int i;
	int *fibo = (int *)malloc(sizeof(int)*(n+1));
	fibo[0] = 0, fibo[1] = 1;
	for(i = 2; i<=n; i++) fibo[i] = fibo[i-1]+fibo[i-2];
	return fibo[n];
}

int max_of_four_int(int a, int b, int c, int d){
	int buf = 0;
	if(a>b) buf = a;
	else buf = b;
	if(buf < c) buf = c;
	if(buf < d) buf = d;
	return buf;
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
void check_vaddr(void *vaddr){
	if(!is_user_vaddr(vaddr))	exit(-1);
}
	

void halt(){
	shutdown_power_off();
}

void exit(int st){
	thread_current()->exit_status = st;
	printf("%s: exit(%d)\n", thread_name(), thread_current()->exit_status);
	thread_exit();
}

tid_t exec(const char *comm){
	return process_execute(comm);
}

int wait(tid_t pid){
  return process_wait(pid);
}

int read(int fd, void* buf, unsigned size){
	int i;
	if(fd == 0){
	  for(i = 0; i < (int)size; i++){
			if(((char *)buf)[i] == '\0') break;
		}
	}
	return i;
}

int write(int fd, void *buf, unsigned size){
	if(fd == 1){
		putbuf(buf, size);
		return size;
	}
	else return -1;
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf("%d", *(uint32_t *)(f->esp));
	//hex_dump(f->esp, f->esp, 100, 1);
	switch(*(uint32_t *)(f->esp)){
		case SYS_HALT:
			halt();
      break;
    case SYS_EXIT:
			check_vaddr(f->esp+4);
			exit(*(uint32_t *)(f->esp+4));
      break;
    case SYS_EXEC:
			check_vaddr(f->esp+4);
			f->eax = exec((const char *)*(uint32_t *)(f->esp+4));
      break;
    case SYS_WAIT:
			check_vaddr(f->esp+4);
			f->eax = wait((tid_t)*(uint32_t *)(f->esp+4));
      break;
    case SYS_CREATE:
      break;
    case SYS_REMOVE:
      break;
    case SYS_OPEN:
      break;
    case SYS_FILESIZE:
      break;
    case SYS_READ:
      check_vaddr(f->esp+4);
			check_vaddr(f->esp+8);
			check_vaddr(f->esp+12);
			f->eax = read((int)*(uint32_t *)(f->esp+4), (void *)*(uint32_t *)(f->esp+8), (unsigned)*((uint32_t *)(f->esp + 12)));
			break;
    case SYS_WRITE:
			check_vaddr(f->esp+4);
			check_vaddr(f->esp+8);
			check_vaddr(f->esp+12);
			f->eax = write((int)*(uint32_t *)(f->esp+4), (void *)*(uint32_t *)(f->esp+8), (unsigned)*((uint32_t *)(f->esp + 12)));
      break;
    case SYS_SEEK:
      break;
    case SYS_TELL:
      break;
    case SYS_CLOSE:
      break;
		case SYS_FIBONACCI:
			check_vaddr(f->esp+4);
			f->eax = fibonacci((int)*(uint32_t *)(f->esp+4));
			break;
		case SYS_FOURMAX:
			check_vaddr(f->esp+4);
			check_vaddr(f->esp+8);
			check_vaddr(f->esp+12);
			check_vaddr(f->esp+16);
			f->eax = max_of_four_int((int)*(uint32_t *)(f->esp+4), (int)*(uint32_t *)(f->esp+8), (int)*(uint32_t *)(f->esp+12), (int)*(uint32_t *)(f->esp+16));
			break;
	}
  //hex_dump(f->esp, f->esp, 100, 1);
  //int syscallNum=*(int*)esp;
  //printf("%d", syscallNum);
  //printf ("system call!\n\n");
  //thread_exit ();
}
