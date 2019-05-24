#ifndef _MEM_H
#define _MEM_H

#include "stdlib.h"
#include "timer.h"

#define GDT_ADDR 0x400000
#define FIFO_ADDR (GDT_ADDR + 256*8)
#define TIMERCTL_ADDR (FIFO_ADDR+sizeof(struct FIFO32))
#define PTASKCTL_ADDR (TIMERCTL_ADDR+sizeof(struct TIMERCTL))
#define PTASKTIMER_ADDR (PTASKCTL_ADDR+4)
#define PSHTBACK_ADDR (PTASKTIMER_ADDR+4)
#define MEMMAN_ADDR (PSHTBACK_ADDR+4)
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

