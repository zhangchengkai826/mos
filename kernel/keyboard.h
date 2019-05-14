#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "stdlib.h"

void asm_inthandler21();
void inthandler21(int *esp);

extern struct FIFO8 keyfifo;

#endif /* _KEYBOARD_H */
