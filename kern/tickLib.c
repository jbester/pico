#include "pico.h"
#include "tickLib.h"

volatile static uint64_t ticks;

extern void schedule(void);

void tickAnnounce() {
	ticks++;
	schedule();
}

void tickSet(uint64_t tick) {
	ticks = tick;
}

uint64_t tickGet() {
	return ticks;
}
