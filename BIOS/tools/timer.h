//*----------------------------------------------------------------------------
// timer.h
//*----------------------------------------------------------------------------
#ifndef __TIMER_H
#define __TIMER_H
//*----------------------------------------------------------------------------
#include "tools.h"
//*----------------------------------------------------------------------------
// This structure is used for declaring a timer. The timer must be set
// with timer_set() before it can be used.
//*----------------------------------------------------------------------------
struct timer {
  vu32 start;
  vu32 interval;
};
//*----------------------------------------------------------------------------
// FUNKTIONEN
//*----------------------------------------------------------------------------
extern void timer_set(struct timer *t, u32 interval);
extern void timer_reset(struct timer *t);
extern void timer_restart(struct timer *t);
extern u32  timer_expired(struct timer *t);
//*----------------------------------------------------------------------------
#endif
