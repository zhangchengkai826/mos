#ifndef _MOUSE_H
#define _MOUSE_H

#include "stdlib.h"

struct MOUSE_DEC {
  unsigned char buf[3], phase, btn;
  int x, y;
};

void enable_mouse(struct MOUSE_DEC *mdec);
void asm_inthandler2c();
void inthandler2c(int *esp);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

#endif /* _MOUSE_H */
