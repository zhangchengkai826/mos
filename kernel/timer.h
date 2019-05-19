#ifndef _TIMER_H
#define _TIMER_H

#include "stdlib.h"

struct TIMERCTL {
  unsigned count;
  unsigned timeout;
  struct FIFO8 *fifo;
  unsigned char data;
};

void init_pit();
void asm_inthandler20();
void inthandler20(int *esp);
void settimer(unsigned timeout, struct FIFO8 *fifo, unsigned char data);

#endif /* _TIMER_H */
