#ifndef _MOUSE_H
#define _MOUSE_H

#include "stdlib.h"

void enable_mouse();

extern struct FIFO8 mousefifo;
void asm_inthandler2c();
void inthandler2c(int *esp);

#endif /* _MOUSE_H */
