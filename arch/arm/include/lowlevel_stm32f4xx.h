#ifndef _LOWLEVEL_STM32F4xx_H_
#define _LOWLEVEL_STM32F4xx_H_


#define DISABLE_INTERRUPTS() asm volatile("mov r0, #1\nmsr PRIMASK, r0\n")
#define ENABLE_INTERRUPTS() asm volatile("mov r0, #0\nmsr PRIMASK, r0\n")

#endif
