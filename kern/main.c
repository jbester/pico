#include "arch/arm/include/stm32f4xx_conf.h"
#include "pico.h"
#include "taskLib.h"
#include "tickLib.h"
#include "config.h"

#define DISABLE_INTERRUPTS() asm volatile("mov r0, #1\nmsr PRIMASK, r0\n")
#define ENABLE_INTERRUPTS() asm volatile("mov r0, #0\nmsr PRIMASK, r0\n")

 
#define MAIN_RETURN 0xFFFFFFF9  //Tells the handler to return using the MSP
#define THREAD_RETURN 0xFFFFFFFD //Tells the handler to return using the PSP
 
//Quick hack, approximately 1ms delay
void ms_delay(int ms)
{
  while (ms-- > 0) {
    volatile int x=5971;
    while (x-- > 0)
      asm volatile("nop");
  }
}


void idle();

/*
  int taskSpawn(char *name, void (*fn)()) {
  int i;
  for(i = 0; i < MAX_TASKS; i++) 
  {
  if (tasks[i].state == TASK_FREE)
  {
  createTcb(&(tasks[i]), fn);
  return i;
  }
  }
  return -1;
  }
*/
void hwInit()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  // enable the clock to GPIOD
  GPIOD->MODER |= (1 << (12 * 2));             // set pin 13 to be general purpose output
  GPIOD->MODER |= (1 << (13 * 2));             // set pin 13 to be general purpose output
  GPIOD->MODER |= (1 << (14 * 2));             // set pin 14 to be general purpose output
  GPIOD->MODER |= (1 << (15 * 2));             // set pin 15 to be general purpose output
  NVIC_SetPriority(SysTick_IRQn, 0);
  NVIC_SetPriority(PendSV_IRQn, 255);
}



void led1() 
{
  for (;;) {
    GPIOD->ODR ^= (1 << 12);           // Toggle the pin 
    ms_delay(250);
  }
}

void led2() 
{
  GPIOD->ODR ^= (1 << 14);           // Toggle the pin 
  for (;;) {
    GPIOD->ODR ^= (1 << 14);           // Toggle the pin 
    taskDelay(250);
  }
}


/*
void __attribute__ ((interrupt)) SVC_Handler(int *arg) {
  
  asm volatile(
	       "MOV r0, %0\n"
	       "ldmia r0!, {r4-r11, r14}\n"
	       "MSR psp, r0\n"
	       "MOV r0, #0\n"
	       "MSR basepri, r0\n"
	       "BX r14\n"
	       : : "r" (tasks[current_task].sp )
	       );

  
    int call = ((char *)arg[6])[-2];
  
}*/

void __attribute__ ((interrupt)) PendSV_Handler(void) {
  asm volatile(
	       "MOV r0, %0\n"
	       "ldmia r0!, {r4-r11, r14}\n"
	       "MSR psp, r0\n"
	       : : "r" (tasks[current_task].sp )
	       );

}


void schedule() {
  DISABLE_INTERRUPTS();

  int previous = current_task;
  do {
    current_task = (current_task + 1)%MAX_TASKS;
    if (current_task != IDLE_TASK && tasks[current_task].state == TASK_RUNNING)
      break;
  } while (current_task != previous);

  if (current_task != IDLE_TASK && current_task == previous && tasks[current_task].state != TASK_RUNNING) {
    current_task = IDLE_TASK; // idle task
  }

  if (tickGet() % 250 == 0) {
    GPIOD->ODR ^= (1 << 15);           

  }
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; 

  ENABLE_INTERRUPTS();
}


void __attribute__ ((interrupt)) SysTick_Handler(void){
  // store current context
  asm volatile(
         "MRS r0, psp\n"
         "stmdb r0!, {r4-r11, r14}\n"
         "MOV %0, r0\n"
         : "=r" (tasks[current_task].sp )
         );
  tickAnnounce();
}


int usrAppInit() 
{
  int i = 0;
  GPIOD->ODR ^= (1 << 13);           // Toggle the pin        
  taskSpawn("led2", 50, 0, 1024, (FUNCPTR)(&led2), 0, 0, 0, 0);
  for(i = 0; i < 100; i ++) {
    taskDelay(10);
    GPIOD->ODR ^= (1 << 13);           // Toggle the pin        
  }
  return 0;
}

static inline void spawnInitialTask() 
{
  current_task = IDLE_TASK;
  asm volatile(
	       "MSR psp, %0\n"
	       "MOV r0, #0x3\n"
	       "MSR CONTROL, r0\n"
	       "B idle\n" : : "r" (tasks[current_task].sp )
	       );
}

void kernelInit() 
{
  //  programStack = (uint32_t)mem_end - DEFAULT_STACK_SIZE;
  tickSet(0);
  taskLibInit();
  hwInit();
  // DEBUG show ON
  // set the pin
  GPIOD->ODR |= (1 << 12);           
  taskSpawn("tIdle", 50, 0, 1024, (&idle), 0, 0, 0, 0);
  taskSpawn("tUsrAppInit", 50, 0, 1024, (&usrAppInit), 0, 0, 0, 0);
  if (SysTick_Config(SystemCoreClock / 1000))
    { 
      // Capture error 
      GPIOD->ODR = 0;           // Toggle the pin          
      while(1) {
      GPIOD->ODR ^= (1 << 15);           // Toggle the pin          
      ms_delay(100);
      }
    }
  spawnInitialTask();
}

void idle() {
  GPIOD->ODR ^= (1 << 12);           // Toggle the pin        
  //  taskSpawn(&led2);

  for(;;) {
    taskDelay(100);
    GPIOD->ODR ^= (1 << 12);           // Toggle the pin        
  }
  /*  while(1)
      asm volatile("nop\n");*/;
}
