#include "mem.h"
#include "lowlevel.h"

#define CR0_CACHE_DISABLE 0x60000000
unsigned memtest_sub(unsigned start, unsigned end);
unsigned memtest(unsigned start, unsigned end) {
  unsigned cr0, i;
  cr0 = load_cr0();
  cr0 |= CR0_CACHE_DISABLE;
  store_cr0(cr0);

  i = memtest_sub(start, end);

  cr0 = load_cr0();
  cr0 &= ~CR0_CACHE_DISABLE;
  store_cr0(cr0);
  return i;
}

void memman_init(struct MEMMAN *man) {
  man->frees = 0;
  man->maxfrees = 0;
  man->lostsize = 0;
  man->losts = 0;
}

unsigned memman_total(struct MEMMAN *man) {
  unsigned i, t = 0;
  for(i = 0; i < man->frees; i++)
    t += man->free[i].size;
  return t;
}

unsigned memman_alloc(struct MEMMAN *man, unsigned size) {
  unsigned i, a;
  for(i = 0; i < man->frees; i++) {
    if(man->free[i].size >= size) {
      a = man->free[i].addr += size;
      man->free[i].size -= size;
      if(man->free[i].size == 0) {
        man->frees--;
        for(; i < man->frees; i++)
          man->free[i] = man->free[i+1];
      }
      return a;
    }
  }
  return 0;
}

int memman_free(struct MEMMAN *man, unsigned addr, unsigned size) {
  int i, j;
  for(i = 0; i < man->frees; i++)
    if(man->free[i].addr > addr)
      break;
  if(i > 0) {
    if(man->free[i-1].addr + man->free[i-1].size == addr) {
      man->free[i-1].size += size;
      if(addr + size == man->free[i].addr) {
        man->free[i-1].size += man->free[i].size;
        man->frees--;
        for(; i < man->frees; i++)
          man->free[i] = man->free[i+1];
      }
      return 0;
    }
  }
  if(i < man->frees) {
    if(addr + size == man->free[i].addr) {
      man->free[i].addr = addr;
      man->free[i].size += size;
      return 0;
    }
  }
  if(man->frees < MEMMAN_FREES) {
    for(j = man->frees; j > i; j--)
      man->free[j] = man->free[j-1];
    man->frees++;
    if(man->maxfrees < man->frees)
      man->maxfrees = man->frees;
    man->free[i].addr = addr;
    man->free[i].size = size;
    return 0;
  }
  man->losts++;
  man->lostsize += size;
  return -1;
}

