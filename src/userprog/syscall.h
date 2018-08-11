#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
bool is_valid(void **esp);
// SYSCALL
int write (int fd, const void *buffer, unsigned size);
void exit(int status); 

#endif /* userprog/syscall.h */
