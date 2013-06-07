#ifndef _TICKLIB_H_
#define _TICKLIB_H_

extern void tickAnnounce();
extern void tickSet(uint64_t tick);
extern uint64_t tickGet();

#endif