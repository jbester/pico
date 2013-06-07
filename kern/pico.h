#ifndef _PICO_H_
#define _PICO_H_

typedef enum 
{
	STATUS_ERROR=0,
	STATUS_OK=1
} status_t;

typedef int (*FUNCPTR)(int, int, int, int);

#endif
