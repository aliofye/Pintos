#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

int write (int fd, const void *buffer, unsigned size);
void exit(int status); 
static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  	//cast f->esp into an int*, then dereference it for the SYS_CODE
	int code = *(int*)f->esp;
	printf("RUN SYSCALL CODE: %i\n", code);
	
	switch(code){
		case SYS_HALT:
			printf("HALT! YOU SHALL NOT PASS\n");
			shutdown_power_off();
			break;

		case SYS_EXIT:
		{
			int code = *((int*)f->esp + 4);
			exit(code);
			break;	
		}
		case SYS_EXEC:
			break;

		case SYS_WAIT:
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
			break;

		case SYS_WRITE:
		{
			int fd = *((int*)f->esp + 1);
			void *buffer = (void*)(*((int*)f->esp + 2));
			unsigned size = *((unsigned*)f->esp + 3);
			//run the syscall, a function of your own making
			//since this syscall returns a value, the return value should be stored in f->eax
			// f->eax = write(fd, buffer, size);
			break;
		}
		case SYS_SEEK:
			break;

		case SYS_TELL:
			break;

		case SYS_CLOSE:
			break;
	}
}

int
write(int fd, const void *buffer, unsigned size) {
	return 0;
}

void 
exit(int status) {
  thread_exit();
}