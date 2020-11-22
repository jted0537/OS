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
	lock_init(&thread_lock);
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
	for(int i = 3; i<128; i++){
			if(thread_current()->file_des[i] != NULL) close(i);
	}
	struct list_elem* elem = list_begin(&thread_current()->child_thread);
	thread_exit();
}

int open(const char *file){
	int i = 0;
	if(file == NULL) exit(-1);
	lock_acquire(&thread_lock);
	struct file *new_file = filesys_open(file);
	if(new_file == NULL)
	{
			lock_release(&thread_lock);
			return -1;
	}
	else{
		for(i = 3; i<128; i++){
			if(thread_current()->file_des[i] == NULL){
				if(strcmp(thread_current()->name, file)==0) file_deny_write(new_file);
				thread_current()->file_des[i] = new_file;
				break;
			}
		}
	}
	lock_release(&thread_lock);
	return i;
}

void close(int fd){
	file_close(thread_current()->file_des[fd]);
	thread_current()->file_des[fd] = NULL;
	return ;
}

int read(int fd, void* buf, unsigned size){
	int i = -1;
	if(!is_user_vaddr(buf) || !is_user_vaddr(buf+size)) exit(-1);
	lock_acquire(&thread_lock);
	if(fd == 0){
	  for(i = 0; i < (int)size; i++){
   		if(input_getc() == '\0') break;
		}
		lock_release(&thread_lock);
		return i;
	}
	//else if(fd == 2) exit(-1);
	else if(fd >= 3){
			if(thread_current()->file_des[fd] == NULL) exit(-1);	
			else i = file_read(thread_current()->file_des[fd], buf, size);
	}
	lock_release(&thread_lock);
	return i;
}

int write(int fd, void *buf, unsigned size){
	int return_value = -1;
	if(!is_user_vaddr(buf) || !is_user_vaddr(buf+size)) exit(-1);
	lock_acquire(&thread_lock);
	if(fd == 1){
		putbuf(buf, size);
		lock_release(&thread_lock);
		return size;
	}
	//else if(fd == 2) exit(-1);
	else if(fd >= 3) {
		if(thread_current()->file_des[fd] == NULL) exit(-1);
		struct file *files = thread_current()->file_des[fd];
		if(files->deny_write) file_deny_write(files);
		return_value = file_write(thread_current()->file_des[fd], buf, size);
	}
	lock_release(&thread_lock);
	return return_value;
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
			f->eax = process_execute((const char *)*(uint32_t *)(f->esp+4));
      break;
    case SYS_WAIT:
			check_vaddr(f->esp+4);
			f->eax = process_wait((tid_t)*(uint32_t *)(f->esp+4));
      break;
    case SYS_CREATE:
      check_vaddr(f->esp+4);
			check_vaddr(f->esp+8);
			if((const char*)*(uint32_t*)(f->esp+4)==NULL) exit(-1);
			f->eax = filesys_create((const char *)*(uint32_t *)(f->esp+4), (unsigned)*(uint32_t *)(f->esp + 8));
			break;
    case SYS_REMOVE:
			check_vaddr(f->esp+4);
			if((const char*)*(uint32_t*)(f->esp+4)==NULL) exit(-1);
			f->eax = filesys_remove((const char*)*(uint32_t *)(f->esp+4));
      break;
    case SYS_OPEN:
			check_vaddr(f->esp+4);
			//if((const char*)*(uint32_t*)(f->esp+4)==NULL) exit(-1);
			f->eax = open((const char *)*(uint32_t *)(f->esp+4));
      break;
    case SYS_FILESIZE:
			check_vaddr(f->esp+4);
			if(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)] == NULL) exit(-1);
			f->eax = file_length(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)]);
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
			check_vaddr(f->esp+4);
			check_vaddr(f->esp+8);
			//if(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)] == NULL) exit(-1);
			file_seek(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)], (unsigned)*(uint32_t *)(f->esp+8));
      break;
    case SYS_TELL:
			check_vaddr(f->esp+4);
			//if(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)] == NULL) exit(-1);
			f->eax = file_tell(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)]);
      break;
    case SYS_CLOSE:
			check_vaddr(f->esp+4);
			//if(thread_current()->file_des[(int)*(uint32_t *)(f->esp+4)] == NULL) exit(-1);
			close((int)*(uint32_t *)(f->esp+4));
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
