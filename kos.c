/*
 * kos.c -- starting point for student's os.
 * 
 */
#include "console_buf.h"
#include <stdlib.h>
#include "simulator.h"
#include "dllist.h"

PCB currentRunningPCB = NULL;



void *initialize_user_process(void *arg){
	PCB thisPCB;
	char **argv1;
	char *name = NULL;
	int argc = 0, i = 0;
    
	int *argptr;  // pointer to arguments


	//User_Limit = MemorySize;
	//User_Base =0;
	//User_Limit = MemorySize-2048;
	//User_Base =1024;
	int partition = get_partition();
    User_Base = partitions[partition].base;
    User_Limit =  partitions[partition].limit; 

	argv1 = (char **)arg;
  
	thisPCB = (PCB) malloc(sizeof(struct ProcessControlBlock));
  
	if (argv1 != NULL) {
        while (argv1[i] != NULL) {
            if (i == 0) name = argv1[i];
            i++;        
        }   
        argc = i;
    }

	for (i=0; i < NumTotalRegs; i++) 
		thisPCB->registers[i] = 0;


	thisPCB->registers[PCReg] = 0; 
	thisPCB->registers[NextPCReg] = 4;
	thisPCB -> base = User_Base;
	thisPCB->sbrk = 0;
	thisPCB->limit = User_Limit;
	thisPCB -> pid = get_new_pid();
  
	if (!perform_execve(thisPCB, name, argv1)) {
        Jval val = new_jval_v(thisPCB);
        dll_prepend(readyq, val);
        kt_exit();
    } else {
        exit(1);
    }	
}



void KOS()
{
	//static char *Argv[5] = { "argtest", "Rex,", "my", "man!", NULL };
	char *Argv = kos_argv;
	readyq = new_dllist();
	writeok = make_kt_sem(0);
	writers = make_kt_sem(1);
	readers = make_kt_sem(1);
	consoleWait = make_kt_sem(0);
    nelem = make_kt_sem(0);
	nslots = make_kt_sem(256);
	console_read_buffer_tail = 0; 
	//run_user_code(registers);
	
	curpid = 0;
	init_partitions(); 
	jrbTree = make_jrb();
	

    console_read_buffer_head = 0;
		bzero(main_memory, MemorySize);
    
    for(int i= 0 ; i< console_read_buffer_size ; i++)
		console_read_buffer[i] = -1;
		
    kt_fork(ConsoleRead, NULL);   
	kt_fork(initialize_user_process, Argv);
	//kt_fork(initialize_user_process, "a.out");
    kt_joinall();
    
	scheduler();
}





int perform_execve(PCB p, char *fn, char **argv) {

	int i = 0,j = 0,k = 0,l=0;
	int topofstack,diff;
    
	int *argptr;  // pointer to arguments
	/*
	if (load_user_program(argv[0]) < 0)
	{
		fprintf(stderr,"Can't load program.\n");
		exit(1);
	}
	*/
p->sbrk = (void*) load_user_program(argv[0]);
	if ((int)p->sbrk < 0){
		fprintf(stderr,"Can't load program.\n");
		return -1;
	}
	while(argv[j] != NULL) // count the number of command line arguments
		j++;

	topofstack =  User_Limit - 20;

	argptr = (int *)malloc(sizeof(int)*j);
  
	for (k=j-1; k >= 0; k--) 
	{
		topofstack = topofstack - (strlen(argv[k])+1);
		argptr[k] = topofstack ;
		strcpy(main_memory+topofstack, argv[k]);
	}
	//topofstack--;
	while(topofstack % 4) 
		topofstack--;

	topofstack =  topofstack - 4;
	l = 0;
	memcpy(main_memory + topofstack, &l, 4);

	// load pointers to strings
	for (k=j-1; k >= 0; k--) 
	{
		topofstack = topofstack - 4;
		l = argptr[k]; 
		memcpy (main_memory + topofstack, &l, 4);
	}

	diff = topofstack ;

	//envp
	topofstack = topofstack - 4;
	l = 0;
	memcpy(main_memory + topofstack, &l, 4); 
	// argv
	topofstack = topofstack - 4;
	l = diff;
	memcpy (main_memory + topofstack, &l, 4);

	// argc
	topofstack = topofstack - 4;
	l = j;
	memcpy(main_memory + topofstack, &l, 4);

	p->registers[StackReg] = topofstack - 12 ;		// stack pointer
	return 0;
}

int get_new_pid() {
    JRB node;
    int i;
    printf("");
    for(i=1;i<32767;i++)
	if((node = jrb_find_int(jrbTree,i)) == NULL)
		break;
    curpid = i;
    DEBUG('e', "Available PID = %d\n", curpid); 
    (void) jrb_insert_int(jrbTree, i, new_jval_i(curpid));
    return curpid;
}

void destroy_pid(int pid) {
    JRB node;
    node = jrb_find_int(jrbTree,pid);
    if(node != NULL) {
        jrb_delete_node(node);
    }	
}

PCB copy_pcb(PCB p) {
   PCB newp = new_pcb();
   int i = 0;
   for (i = 0; i < NumTotalRegs; i++) {
       newp->registers[i] = p->registers[i];
   }
   newp->writebuffer = NULL;
   newp->writesize = 0;
   newp->charswritten = 0;
   
   newp->readbuffer = NULL;
   newp->charsread = 0;
   newp->readsize = 0;
   
   newp->sbrk = p->sbrk;
   newp->base = p->base;
   newp->limit = p->limit;
 
   newp->pid = p->pid;   
   return newp;
}

PCB new_pcb() {
    int i = 0;
    PCB p = (PCB )malloc(sizeof(PCB));
    for (i = 0; i < NumTotalRegs; i++) {
        p->registers[i] = 0;
    }    
    p->writebuffer = NULL;
   p->writesize = 0;
   p->charswritten = 0;
   
   p->readbuffer = NULL;
   p->charsread = 0;
   p->readsize = 0;
   
    p->sbrk = 0;
    p->base = 0;
    p->limit = 0;
 
    p->pid = 0;
    p->mem_partition = 0; 
   

    return p;
}