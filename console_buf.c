#include "console_buf.h"

void *ConsoleRead()
{
	/*
	int i;
	while(1)
	{
		P_kt_sem(consoleWait);
		if(console_read_buffer_tail == -1)
		{
			console_read_buffer_tail = 0;
			console_read_buffer_head = 0;
			console_read_buffer[console_read_buffer_tail] = console_read();
			V_kt_sem(nelem);
		}	
		else if((console_read_buffer_tail+1)%256 != console_read_buffer_head)
		{
			console_read_buffer_tail = (console_read_buffer_tail + 1) % 256;
			console_read_buffer[console_read_buffer_tail] = console_read();
			V_kt_sem(nelem);
		}
	}
*/	

	while(1){
		P_kt_sem(consoleWait);
		P_kt_sem(nslots);
		// Do the real reading
		// V (nelems)
		console_read_buffer_tail;
		console_read_buffer[console_read_buffer_tail] = console_read();
		console_read_buffer_tail ++;
		console_read_buffer_tail %= 256;
		//printf("%d", console_read_buffer_tail);

		V_kt_sem(nelem);
	}
}

void console_log(){
    int c;
    for (int i = 0; i < kt_getval(nelem); i++){
        c = *(console_read_buffer + (console_read_buffer_head + i) % 256);
        //c = *((console_read_buffer_tail + i) % 256);
        printf ("%d\n",c);
        //printf ("%d\n",);
    }
	
   
}
