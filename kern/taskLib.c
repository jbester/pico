#include "pico.h"
#include "config.h"
#include "sysLib.h"
#include "taskLib.h"

#define THREAD_RETURN 0xFFFFFFFD //Tells the handler to return using the PSP

static char* stack;
tcb_t tasks[MAX_TASKS];
int current_task = 0;

int taskIdSelf() {
  return current_task;
}

void taskLibInit() {
  int i;
  stack = sysMemTop() - DEFAULT_STACK_SIZE;
  current_task = 0;
  for(i = 0; i < MAX_TASKS; i++) {
    tasks[i].state = TASK_FREE;
  }
}

static uint32_t *allocateThreadStack(uint32_t stackSize) 
{
  // TODO smarter allocation
  uint32_t *result = (uint32_t *)stack;
  stack -= stackSize;
  return result;
}

void taskInit(
    tcb_t *    tcb, 
    char *     name,          // name of new task (stored at stack base in tcb) 
    int        priority,      // priority of new task 
    uint32_t   options,       // task option word 
    void *     stackBase,    // base of new task's stack 
    int        stackSize,     // size (bytes) of stack needed 
    FUNCPTR    entryPoint,       // entry point of new task 
    int        arg1,          // arguments
    int        arg2,
    int        arg3,
    int        arg4 ) {
  char *base = (char*)stackBase;
  uint32_t *sp;

  // copy task name to base of stack
  while((*(--base) = *name++) != 0);

  // align stack
  do {
    *(--base) = 0;
  } while(((uint32_t)base % 4) !=0);

  // create initial stack frame
  sp = (uint32_t*)base;
  sp--;
  *sp = 0x01000000; // xPSR 
  sp--;
  *sp = (uint32_t)entryPoint;
  sp--;
  *sp = (uint32_t)exit;  // LR 

  sp --;  // R12 
  sp --;  // R3 
  *sp = arg4;  
  sp --;  // R2 
  *sp = arg3;  
  sp --;  // R1 
  *sp = arg2;  
  sp --;  // R0 
  *sp = arg1;  

  // using interrupts so
  sp--;
  *sp = THREAD_RETURN;

  sp -= 8;  // R11, R10, R9, R8, R7, R6, R5 and R4. 
  tcb -> state = TASK_RUNNING;
  tcb -> sp = (uint32_t)sp;
  tcb -> stackBase = (uint32_t)stackBase;
  tcb -> stackSize = stackSize;
  tcb -> entryPoint = entryPoint;
  tcb -> options = options;
  tcb -> priority = priority;
} 


int taskSpawn(char *name,     
        int     priority,         // priority of new task 
        int     options,          // task option word 
        int     stackSize,        // size (bytes) of stack needed plus name 
        FUNCPTR entryPoint,          // entry point of new task 
        int     arg1,             // arguments
        int     arg2,
        int     arg3,
        int     arg4) {
  int i;
  for(i = 0; i < MAX_TASKS; i++) 
  {
    if (tasks[i].state == TASK_FREE)
    {
      uint32_t *sp = allocateThreadStack(stackSize);
      taskInit(&(tasks[i]), name, priority, options, sp, stackSize, entryPoint, arg1, arg2, arg3, arg4);
      return i;
    }
  }
  return -1;
}

status_t taskDelete(int tid) 
{
  tasks[tid].state = TASK_FREE;
  while(1);
}

extern volatile uint64_t ticks;

void taskDelay(uint32_t delay) 
{
  // TODO interface with scheduler
  uint64_t end = tickGet() + delay;
  while(tickGet() < end);
}

void exit(int errorCode) 
{
  tasks[current_task].state = TASK_FREE;
  while(1);
}
