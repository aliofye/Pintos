#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void is_valid(void *ptr);
void is_bad(void *ptr);
// SYSCALL
int write (int fd, const void *buffer, unsigned size);
void exit(int status); 
bool create(const char* file, unsigned initial_size);
bool remove(const char* file);
int open(const char* file);
// pid_t exec(const char* cmd_line);

#endif /* userprog/syscall.h */
