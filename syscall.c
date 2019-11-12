#include "syscall.h"

void sys_return(PCB currentpcb, int x)
{
	DEBUG('e',"sys_return Called.\n");
	currentpcb->registers[PCReg] = currentpcb->registers[NextPCReg]; // Set PCReg in the saved registers to NextPCReg.  
				// If you don't do this, the process will keep calling the system call. 
	currentpcb->registers[2] = x;  // Put the return value into the stored register 2
    //printf("%d\n",dll_size(readyq));
	dll_prepend(readyq,new_jval_v((void *)currentpcb));	// push the PCB onto the ready queue
    
	kt_exit();
}



void* do_write(void *pv) {
   int ret = 0;
   PCB p = (PCB )pv;
   if (p->registers[5] != 1 && p->registers[5] != 2) {
       //fprintf(stdout, "arg1 %s\n", p->rgs[5]);
       sys_return(p, 0 - EBADF);
   }
   //fprintf(stdout, "mem %d, loc %d\n", MemorySize, p->rgs[6]);
   if (p->registers[6] < 0 || p->registers[6] + p->registers[7] > (p->limit)) {
       //fprintf(stdout, "arg1 %s\n", p->rgs[6]);
       sys_return(p, 0 - EFAULT);
   }
   if (p->registers[7] < 0) {
       sys_return(p, 0 -EINVAL);
   }
   DEBUG('e', "1111111111\n");
   p->writebuffer = main_memory + p->base + p->registers[6];    
   p->writesize = p->registers[7];   
   P_kt_sem(writers);
   while (p->writesize != p->charswritten) {
       console_write(p->writebuffer[p->charswritten++]);
       P_kt_sem(writeok);
   } 
   V_kt_sem(writers);
   DEBUG('e', "2222222222\n");
   p->writebuffer = main_memory + p->base + p->registers[6];    
   p->writebuffer = NULL;
   p->writesize = 0;
   ret = p->charswritten;
   p->charswritten = 0;
   sys_return(p, ret);
}



void* do_read(void *arg){
    PCB thisPCB = (PCB)arg;
    int retureVal = 0;
    //P_kt_sem(readers);
    //V_kt_sem(readers);

			//P_kt_sem(nelem);
            // do rewal read
            //V_kt_sem(nslots);
    //V_kt_sem(nslots)

    //P_kt_sem(nelem);	
    
    //if(thisPCB->registers[5] != 1 && thisPCB->registers[5] != 2)
        //retureVal = -EBADF;
       // sys_return(thisPCB, -EBADF);
        
    if(thisPCB->registers[5] != 0)
		sys_return(thisPCB, -EBADF);

    if(thisPCB->registers[6] < 0) 
        sys_return(thisPCB, -EFAULT);

    int address = thisPCB->registers[6] + User_Base;
    if (!validAddress(address)) {
        sys_return(thisPCB, - EINVAL);
    }

    if(thisPCB->registers[7] < 0)
		sys_return(thisPCB, -EINVAL);

    thisPCB->cptr = main_memory + thisPCB->registers[6];
    /*
    P_kt_sem(readers);
    if(thisPCB->size > 0)
	{
		while(thisPCB->charswritten < thisPCB->size)
		{
			console_write(thisPCB->cptr[thisPCB ->charswritten]);
			P_kt_sem(writeok);
			thisPCB ->charswritten++;
		}
	}
    V_kt_sem(readers);
	retureVal = thisPCB->charswritten;
	DEBUG('e', "No of Characters written = %d\n", retureVal);
	*/
    thisPCB->readbuffer = main_memory + thisPCB->registers[6];
	thisPCB->readsize = thisPCB->registers[7];
	thisPCB->charsread = 0;
    if(thisPCB->readsize > 0)
	{
		while(thisPCB->charsread < thisPCB->readsize)
		{
            P_kt_sem(nelem);
            if(console_read_buffer[console_read_buffer_head] ==-1)
				break;
            //int c = console_read_buffer[console_read_buffer_head];
            thisPCB->readbuffer[thisPCB->charsread] = console_read_buffer[console_read_buffer_head];
            console_read_buffer[console_read_buffer_head] = -1;
			
		    console_read_buffer_head ++;
		    console_read_buffer_head %= 256;
			thisPCB->charsread++; 
            V_kt_sem(nslots);
            //retureVal = thisPCB->charsread;
            //if (c==-1){
              //  retureVal = -1;
                //break;
            //}

        }
    }
    sys_return(thisPCB,thisPCB->charsread);
}



void* do_ioctl(void* arg){
    PCB p = (PCB )arg;
    int address = p->registers[6] + User_Base;
    if (!validAddress(address)) {
        sys_return(p, - EINVAL);
    }
    if (p->registers[5] != 1) {
       sys_return(p, - EINVAL);
    }
    if (p->registers[6] != KOS_TCGETP) {
        sys_return(p, - EINVAL);
    }
    if (p->registers[7] <0) {
       sys_return(p, - EINVAL);
   }
    
   ioctl_console_fill(main_memory + (p->base) + (p->registers[7]));
   sys_return(p, 0);
}



void* do_fstat(void *arg) {
    PCB p = (PCB ) arg;

    int address = p->registers[6] + User_Base;
    if (!validAddress(address)) {
        sys_return(p, - EINVAL);
    }
    if (p->registers[5] <0) {
        sys_return(p, - EINVAL);
    }
    if (p->registers[6] != KOS_TCGETP) {
        sys_return(p, - EINVAL);
    }
    if (p->registers[7] <0) {
        sys_return(p, - EINVAL);
    }
    int size = 0; 
    if (p->registers[5] == 0) {
        size = 1;
    } else if (p->registers[5] == 1 || p->registers[5] == 2) {
        size = 256;
    }
    stat_buf_fill(main_memory + p->base + p->registers[7],size);
    sys_return(p, 0);
}



void* do_getpagesize(void *arg) {
   PCB p = (PCB )arg;
   sys_return(p, PageSize);
}



void* do_sbrk(void *arg) {
    PCB p = (PCB )arg;

    int address = p->registers[6] + User_Base;
    if (!validAddress(address)) {
        sys_return(p, - EINVAL);
    }

    if (p->registers[5] < 0) {
        sys_return(p,  - EINVAL);
    }
    int incr = p->registers[5];
    if ((p->sbrk + incr) > (p->limit)) {
        sys_return(p,  - EINVAL); 
    }
    if ((p->sbrk + incr) <= 0) {
        sys_return(p,  - EINVAL); 
    }
    int temp = p->sbrk;
    p->sbrk = p->sbrk + p->registers[5];       
    sys_return(p, temp);
}



void* do_execve(void *pv) {
    
   PCB p = (PCB )pv;
   char **argv = (char **)(main_memory + p->base + p->registers[6]); 
//copy name
   char *name = (char *) malloc(sizeof(char) * (strlen(main_memory + p->base + p->registers[5]) + 1));
   memcpy(name, main_memory + p->base + p->registers[5], strlen(main_memory + p->base + p->registers[5]) + 1);
   
   
   DEBUG('v', "name %s\n", name);
   int i = 0, argc = 0; 
   if (argv != NULL) {
       while (argv[i] != NULL) {
          DEBUG('v', "argv %s\n", main_memory + p->base + (int)argv[i]);
          i++;        
       }   
       argc = i;
   }
   char **argvcopy = (char **) malloc(sizeof(char *) * (argc + 1));
   i = 0;
   if (argv != NULL) {
       while (argv[i] != NULL) {
          argvcopy[i] = (char *) malloc(sizeof(char) * (strlen(main_memory + p->base + (int)argv[i]) + 1));
          memcpy(argvcopy[i], main_memory + p->base + (int)argv[i], (strlen(main_memory + p->base + (int)argv[i]) + 1)); 
          DEBUG('v', "argv %s\n", argvcopy[i]); 
          //printf();
          i++;        
       }   
       argvcopy[i] = NULL;
   }
   //printf(name);
   //printf()
   int ret = 0;
   if (!(ret = perform_execve(p, name, argvcopy))) {
       
       sys_return_new(p, 0);
       //printf("entered first case\n");
       free(argvcopy);
       free(name);
       
      /*
       p->registers[PCReg] = 0;
       p->registers[NextPCReg] = 4;
       Jval val = new_jval_v(p);
       dll_prepend(readyq, val);
       free(argvcopy);
       free(name);
       kt_exit();
       */
       
   } else { 
       sys_return(p, ret);
       //printf("entered\n");
       free(argvcopy);
       free(name);
   }
}




void sys_return_new(PCB p, int ret) {
   //int buf[NumTotalRegs];
   p->registers[2] = ret; 
   p->registers[PCReg] = 0;
   p->registers[NextPCReg] = 4;
   Jval val = new_jval_v(p);
   dll_prepend(readyq, val);
   kt_exit();
}

bool validAddress(int addr) {
    return (addr < User_Base+User_Limit && addr >= User_Base);
}

void* do_getpid(void *pv) {
    PCB p = (PCB )pv;
    sys_return(p, p->pid);
}

void* do_fork(void *pv) {
   PCB p = (PCB )pv;
	

   int partition = get_partition();
   if (partition == -1) {
       sys_return(p, 0 - EAGAIN);
   }

   PCB newp = copy_pcb(p);
   DEBUG('e', "********* partition %d, base %d limit %d", partition, partitions[partition].base, partitions[partition].limit);
   newp->limit = partitions[partition].limit;
   newp->base = partitions[partition].base;
   newp->mem_partition = partition;
   newp->parent = p;

   newp->pid = get_new_pid();
   DEBUG('e', "pid %d\n", newp->pid);
   memcpy(main_memory + newp->base, main_memory + p->base, p->limit);


   //kt_fork(finish_work, newp);
   sys_return(p, newp->pid);
}






