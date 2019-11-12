/*
 * exception.c -- stub to handle user mode exceptions, including system calls
 * 
 * Everything else core dumps.
 * 
 * Copyright (c) 1992 The Regents of the University of California. All rights
 * reserved.  See copyright.h for copyright notice and limitation of
 * liability and disclaimer of warranty provisions.
 */
#include <stdlib.h>
#include "simulator.h"
#include "syscall.h"
#include "scheduler.h"
#include "console_buf.h"


void
exceptionHandler(ExceptionType which)
{
	int             type, r5, r6, r7, newPC;
	int             buf[NumTotalRegs];

	examine_registers(buf);
	type = buf[4];
	r5 = buf[5];
	r6 = buf[6];
	r7 = buf[7];
	newPC = buf[NextPCReg];

	if(currentRunningPCB != NULL)
	{
		for (int i=0; i < NumTotalRegs; i++)
			currentRunningPCB->registers[i] = buf[i];
	}

	//PCB *CP = currentRunningPCB;

	/*
	 * for system calls type is in r4, arg1 is in r5, arg2 is in r6, and
	 * arg3 is in r7 put result in r2 and don't forget to increment the
	 * pc before returning!
	 */
	DEBUG('e', "Exception Handler Called. Type %d\n",type);
	switch (which) {
	case SyscallException:
		/* the numbers for system calls is in <sys/syscall.h> */
		switch (type) {
		case 0:
			/* 0 is our halt system call number */
			DEBUG('e', "Halt initiated by user program\n");
			SYSHalt();
		case SYS_exit:
			/* this is the _exit() system call */
			DEBUG('e', "_exit() system call\n");
			//console_log();
            printf("Program exited with value %d.\n", r5);
			SYSHalt();
		case SYS_write:
			DEBUG('e', "write system call\n");
			kt_fork(do_write,(void *)currentRunningPCB);
			break;
		case SYS_read:
			DEBUG('e', "read system call\n");
			kt_fork(do_read,(void *)currentRunningPCB);
			break;
			
		case SYS_ioctl:
            DEBUG('e', "ioctl sys call\n");
			
				kt_fork(do_ioctl, (void *)currentRunningPCB);
	
			break;

		case SYS_fstat:
            DEBUG('e', "fstat sys call\n");

            kt_fork(do_fstat, (void *)currentRunningPCB);
            break;

		case SYS_getpagesize:
            DEBUG('e', "getpagesize sys call\n");

            kt_fork(do_getpagesize, (void *)currentRunningPCB);
            break;
			
		case SYS_sbrk:
            DEBUG('e', "sbrk sys call\n");
            
                kt_fork(do_sbrk, (void *)currentRunningPCB);
            
            break;
		case SYS_execve:
            DEBUG('e', "execve sys call\n");
            kt_fork(do_execve, (void *)currentRunningPCB);
            break;

		case SYS_getppid:
                        DEBUG('e', "getppid sys call\n");
                       
                            kt_fork(do_getpid, (void *)currentRunningPCB);
                        
                        break;

		case SYS_fork:
                        DEBUG('e', "fork sys call\n");
                
                        kt_fork(do_fork, (void *)currentRunningPCB);
                        
                        break;

		default:
			DEBUG('e', "Unknown system call\n");
			//printf("%d\n", type);
			SYSHalt();
			break;
		}
		break;
	case PageFaultException:
		DEBUG('e', "Exception PageFaultException\n");
		break;
	case BusErrorException:
		DEBUG('e', "Exception BusErrorException\n");
		break;
	case AddressErrorException:
		DEBUG('e', "Exception AddressErrorException\n");
		break;
	case OverflowException:
		DEBUG('e', "Exception OverflowException\n");
		break;
	case IllegalInstrException:
		DEBUG('e', "Exception IllegalInstrException\n");
		break;
	default:
		printf("Unexpected user mode exception %d %d\n", which, type);
		exit(1);
	}
	kt_joinall();
	scheduler();
	//noop();
}

void
interruptHandler(IntType which)
{

	int buf[NumTotalRegs],i;
	PCB programpcb;

	examine_registers(buf);

	if(currentRunningPCB != NULL)
	{
		for (i=0; i < NumTotalRegs; i++)
			currentRunningPCB->registers[i] = buf[i];
		dll_append(readyq,new_jval_v(currentRunningPCB));
	}

	switch (which) {
	case ConsoleReadInt:
		DEBUG('e', "ConsoleReadInt interrupt\n");
		V_kt_sem(consoleWait);
		
		//noop();
        //console_read_buffer_head = -1;
        //console_read_buffer_tail = -1;
		//kt_getval();
		/*
        while (console_read_buffer[console_read_buffer_head] != -1){
            if(console_read_buffer_head == console_read_buffer_tail)
            {
                console_read_buffer_head = -1;
                console_read_buffer_tail = -1;
            }
            else{
                console_read_buffer_head = (console_read_buffer_head+ 1) % 256;
                printf("%d", console_read_buffer[console_read_buffer_head]);
            }
        }
		*/
		break;
	case ConsoleWriteInt:
		DEBUG('e', "ConsoleWriteInt interrupt\n");
		//noop();
		V_kt_sem(writeok);
		break;
	default:
		DEBUG('e', "Unknown interrupt\n");
		//noop();
		break;
	}
	kt_joinall();
	scheduler();
}
