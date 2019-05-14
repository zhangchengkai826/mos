#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "stdlib.h"

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064

void asm_inthandler21();
void inthandler21(int *esp);
void wait_KBC_sendready();
void init_keyboard();

extern struct FIFO8 keyfifo;

#endif /* _KEYBOARD_H */
