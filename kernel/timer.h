#ifndef _TIMER_H
#define _TIMER_H

#include "stdlib.h"
#define MAX_TIMER 500

struct TIMER {
  unsigned timeout, flags;
  struct FIFO32 *fifo;
  int data;
};
struct TIMERCTL {
  unsigned count, next, using;
  struct TIMER *timers[MAX_TIMER];
  struct TIMER timers0[MAX_TIMER];
};

void init_pit();
void asm_inthandler20();
void inthandler20(int *esp);
struct TIMER *timer_alloc();
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned timeout);

#endif /* _TIMER_H */
