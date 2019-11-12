#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "kos.h"

void sys_return(PCB, int);

void sys_return_new(PCB, int);

void* do_write(void*);

void* do_read(void*);

void* do_ioctl(void*);

void* do_fstat(void*);

void* do_getpagesize(void*);

void* do_sbrk(void *);

void* do_execve(void *);

bool validAddress(int);



void* do_getpid(void *);
void* do_fork(void *);
#endif
