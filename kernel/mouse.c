#include "mouse.h"
#include "keyboard.h"
#include "lowlevel.h"

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

void enable_mouse() {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
}

struct FIFO8 mousefifo;
void inthandler2c(int *esp) {
  unsigned char data;
  io_out8(PIC1_OCW2, 0x64);
  io_out8(PIC0_OCW2, 0x62);
  data = io_in8(PORT_KEYDAT);
  fifo8_put(&mousefifo, data);
}

