#ifndef SCHEDULER_C
#define SCHEDULER_C

#include "dllist.h"
#include "jval.h"
//#include "kos.h"
#include "simulator.h"
#include "scheduler.h"

#endif

void scheduler() {

  Dllist firstDllist;
  PCB thisPCB;
  if(!dll_empty(readyq)) {
    firstDllist = dll_first(readyq);
    thisPCB = jval_v(dll_val(firstDllist));
    dll_delete_node(firstDllist);
    currentRunningPCB = thisPCB;
    DEBUG('e',"\n Removed first process from the ready queue \n");
    run_user_code(thisPCB->registers);
  }
  else {
    currentRunningPCB = NULL;
    DEBUG('e',"\n Ready Queue empty \n");
    noop();
  }
}
