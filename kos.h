#ifndef KOS_H
#define KOS_H

#include "simulator.h"
#include "kt.h"
#include "dllist.h"
#include "errno.h"
#include "jrb.h"
#include "rb.h"
#include "memory.h"
#include <stdlib.h>

typedef struct ProcessControlBlock {

  	char *cptr;
	int size;
	int charswritten;
	int charsread;
	int registers[NumTotalRegs];
    int readsize;
	int writesize;
	int base;
	char *readbuffer;
	char *writebuffer;
	int sbrk;
	int limit;
	unsigned short pid;

	struct ProcessControlBlock *parent;
	int mem_partition;

}*PCB;



Dllist readyq;
JRB jrbTree;
unsigned short curpid;
Rb_node pids;

extern PCB currentRunningPCB;
kt_sem writeok;
kt_sem writers;
kt_sem readers;

kt_sem consoleWait;
kt_sem nelem;
kt_sem nslots;



void *initialize_user_process(void *arg);
//void* initialize_user_process(char *arg);

void KOS();
int perform_execve(PCB p, char *fn, char **argv);

#define console_read_buffer_size 256
int console_read_buffer[console_read_buffer_size];
int console_read_buffer_head;
int console_read_buffer_tail;

PCB new_pcb();
PCB copy_pcb();

int get_new_pid();
void destroy_pid(int ) ;



#endif
