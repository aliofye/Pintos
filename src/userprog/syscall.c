#include <stdio.h>
#include <stdbool.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	// Check if stack is valid
	if(!is_valid(f->esp)){
		exit(-1);
	}

	int code = *(int*)f->esp;
	
	// printf("SYSCALL: %i\n", code);
	
	switch(code){
		case SYS_HALT:
			// printf("HALT\n");
			
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
			printf("WAIT\n");

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
			// printf("WRITE\n");

			int fd = *((int*)f->esp + 1);
			void *buffer = (void*)(*((int*)f->esp + 2));
			unsigned size = *((unsigned*)f->esp + 3);
						
			f->eax = write(fd, buffer, size);
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

bool 
is_valid(void **esp){
	if(esp < 0x08048000 || esp > PHYS_BASE){
		return false;
	}

	return true;
}

int
write(int fd, const void *buffer, unsigned size) {
 	// if file descriptor is STDOUT, write to console
 	if(fd == STDOUT_FILENO){
 		putbuf(buffer, size);
 		return size;
 	}
}

void 
exit(int status) {
  printf("%s: exit(%i)\n", thread_current()->name, status);
  thread_exit();
}