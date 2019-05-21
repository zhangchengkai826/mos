#ifndef _MEM_H
#define _MEM_H

#include "stdlib.h"
#include "timer.h"

#define FIFO_ADDR 0x400000
#define TIMERCTL_ADDR (FIFO_ADDR+sizeof(struct FIFO32))
#define MEMMAN_ADDR (TIMERCTL_ADDR+sizeof(struct TIMERCTL))
#define MEMMAN_FREES 4090

struct FREEINFO {
  unsigned addr, size;
};

struct MEMMAN {
  int frees, maxfrees, lostsize, losts;
  struct FREEINFO free[MEMMAN_FREES];
};

unsigned memtest(unsigned start, unsigned end);
void memman_init(struct MEMMAN *man);
unsigned memman_total(struct MEMMAN *man);
unsigned memman_alloc(struct MEMMAN *man, unsigned size);
int memman_free(struct MEMMAN *man, unsigned addr, unsigned size);
unsigned memman_alloc_4k(struct MEMMAN *man, unsigned size);
int memman_free_4k(struct MEMMAN *man, unsigned addr, unsigned size);

#endif /* _MEM_H */

