#define DEBUG 0

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "devices/shutdown.h"
#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "filesys/file.h"

static void syscall_handler (struct intr_frame *);
struct lock fs_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&fs_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	is_valid(f->esp);

	int code = *(int*)f->esp;
	

	#if DEBUG
	printf("SYSCALL: %i\n", code);
	#endif
	
	switch(code){
		case SYS_HALT:
			
			#if DEBUG
			printf("HALT\n");
			#endif
			
			shutdown_power_off();
			break;

		case SYS_EXIT:
		{	
			int *code = *((int*)f->esp + 1);
		
			#if DEBUG
			printf("EXIT\n");
			printf("CODE: %i\n", code);
			#endif

			exit(code);
			break;	
		}
		case SYS_EXEC:
		{
			#if DEBUG
			printf("EXEC\n");
			#endif

			int *command = *((int *) f->esp + 1);
			is_valid(command);
			is_bad(command);

			break;
		}
		case SYS_WAIT:
			#if DEBUG
			printf("WAIT\n");
			#endif

			break;

		case SYS_CREATE:
		{	
			int *file = *((int *) f->esp + 1);
			unsigned size = *((unsigned*)f->esp + 2);

			is_bad(file);

			#if DEBUG
			printf("CREATE\n");
			printf("FILE NAME: %s\n", (char *) file);
			printf("FILE SIZE: %u\n", size);
			#endif

			bool success = create(file, size);
			f->eax = success;

			break;
		}
		case SYS_REMOVE:
		{	
			int *file = *((int *) f->esp + 1);
			is_bad(file);

			#if DEBUG
			printf("REMOVE\n");
			printf("FILE NAME: %s\n", (char *) file);
			#endif

			bool success = remove(file);
			f->eax = success;
			break;
		}

		case SYS_OPEN:
		{
			int *file = *((int *) f->esp + 1);
			is_bad(file);

			#if DEBUG
			printf("OPEN\n");
			printf("FILE NAME: %s\n", (char *) file);
			#endif

			int fd = open(file);

			f->eax = fd;

			break;
		}
		case SYS_FILESIZE:
			break;

		case SYS_READ:
		{
			#if DEBUG
			printf("READ\n");
			#endif

			int fd = *((int*)f->esp + 1);
			void *buffer = (void*)(*((int*)f->esp + 2));
			unsigned size = *((unsigned*)f->esp + 3);
			
			is_valid(buffer);
			is_bad(buffer);

			
			break;
		}
		case SYS_WRITE:
		{
			#if DEBUG
			printf("WRITE\n");
			#endif

			int fd = *((int*)f->esp + 1);
			void *buffer = (void*)(*((int*)f->esp + 2));
			unsigned size = *((unsigned*)f->esp + 3);
			
			is_valid(buffer);
			is_bad(buffer);

			f->eax = write(fd, buffer, size);
			break;
		}
		case SYS_SEEK:
			break;

		case SYS_TELL:
			break;

		case SYS_CLOSE:
			
			#if DEBUG
			printf("CLOSE\n");
			#endif

			break;
	}
}

void 
is_valid(void *ptr){
	
	if(ptr < (void *) 0x08048000 || ptr > PHYS_BASE || ptr == NULL){
		exit(-1);
	}

	return;
}

void
is_bad(void *ptr){

	void *page = pagedir_get_page (thread_current()->pagedir, ptr);
	if(page == NULL){
		exit(-1);
	}

	return;
}

int
write(int fd, const void *buffer, unsigned size) {
 	// if file descriptor is STDOUT, write to console
 	if(fd == STDOUT_FILENO){
 		putbuf(buffer, size);
 		return size;
 	}

 	if(fd != NULL){
 		file_write(fd, buffer, size);
 	}
}

void 
exit(int status) {
  printf("%s: exit(%i)\n", thread_current()->name, status);
  thread_exit();
}

bool
create(const char* file, unsigned initial_size){
	if(file == NULL || file[0] == '\0'){
		exit(-1);
	}

	if(strlen(file) >= 511){
		return false;
	}

	lock_acquire(&fs_lock);
	bool success = filesys_create(&file, initial_size);
	lock_release(&fs_lock);

	return success;
}

bool
remove(const char* file){
	bool success = filesys_remove(file);
	return success;
}

int
open(const char* file){
	
	struct file *f = filesys_open(file);

	if(file[0] == '\0' || f == NULL){
		return -1;
	}

	return 3;
}

// pid_t exec(const char* cmd_line){
// 	pid_t p = 0;
// 	return p;
// }