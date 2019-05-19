#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "stdlib.h"

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064

#define KEYCMD_WRITE_OUTPUT 0xd1
#define KEYCMD_WRITE_MODE 0x60

void asm_inthandler21();
void inthandler21(int *esp);
void wait_KBC_sendready();
void init_keyboard();

#endif /* _KEYBOARD_H */
