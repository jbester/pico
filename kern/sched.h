#ifndef __SCHED_H__
#define __SCHED_H__

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
  uint32_t entryPoint;
  task_state_t state;
  uint32_t options;
} tcb_t;
/*
void restore_context(tcb_t *tcbp);
void save_context(tcb_t *tcbp);
void initialize_tcb(tcb_t *tcbp, void* sp);
*/
#endif
