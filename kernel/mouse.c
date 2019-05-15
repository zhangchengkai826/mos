#include "mouse.h"
#include "keyboard.h"
#include "lowlevel.h"

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

void enable_mouse(struct MOUSE_DEC *mdec) {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  mdec->phase = 0;
}

struct FIFO8 mousefifo;
void inthandler2c(int *esp) {
  unsigned char data;
  io_out8(PIC1_OCW2, 0x64);
  io_out8(PIC0_OCW2, 0x62);
  data = io_in8(PORT_KEYDAT);
  fifo8_put(&mousefifo, data);
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat) {
  if(mdec->phase == 0) {
    if(dat == 0xfa) {
      mdec->phase = 1;
    }
    return 0;
  }
  if(mdec->phase == 1) {
    mdec->buf[0] = dat;
    mdec->phase = 2;
    return 0;
  }
  if(mdec->phase == 2) {
    mdec->buf[1] = dat;
    mdec->phase = 3;
    return 0;
  }
  if(mdec->phase == 3) {
    mdec->buf[2] = dat;
    mdec->phase = 1;
    return 1;
  }
  return -1;
}

