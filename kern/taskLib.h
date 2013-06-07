#ifndef _TASKLIB_H_
#define _TASKLIB_H_

#include "pico.h"

#include <stdint.h>

typedef enum  {
  TASK_FREE = 0x0,
  TASK_RUNNING = 0x1,
} task_state_t;


typedef struct tcb {
  char *taskName;
  uint32_t sp;
  uint32_t priority;
  uint32_t stackBase;
  uint32_t stackSize;
  FUNCPTR entryPoint;
  task_state_t state;
  uint32_t options;
} tcb_t;

extern tcb_t tasks[];
extern int current_task;

extern status_t taskDelete();

extern void exit(int);

extern int taskSpawn(char *name,     
		     int     priority,         // priority of new task 
		     int     options,          // task option word 
		     int     stackSize,        // size (bytes) of stack needed plus name 
		     FUNCPTR entryPoint,          // entry point of new task 
		     int     arg1,             // 1st of 10 req'd task args to pass to func 
		     int     arg2,
		     int     arg3,
		     int     arg4);
extern void taskInit(
		     tcb_t *    tcb, 
		     char *     name,          /* name of new task (stored at pStackBase) */
		     int        priority,      /* priority of new task */
		     uint32_t   options,       /* task option word */
		     void *     stackBase,    /* base of new task's stack */
		     int        stackSize,     /* size (bytes) of stack needed */
		     FUNCPTR    entryPoint,       /* entry point of new task */
		     int        arg1,          /* first of four task args to pass to func */
		     int        arg2,
		     int        arg3,
		     int        arg4 );

extern void taskLibInit();

#endif
