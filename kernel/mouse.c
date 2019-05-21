#include "mouse.h"
#include "keyboard.h"
#include "lowlevel.h"
#include "mem.h"

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

void enable_mouse(struct MOUSE_DEC *mdec) {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  mdec->phase = 0;
}

void inthandler2c(int *esp) {
  int data;
  struct FIFO32 *fifo = (struct FIFO32 *)FIFO_ADDR;
  io_out8(PIC1_OCW2, 0x64);
  io_out8(PIC0_OCW2, 0x62);
  data = io_in8(PORT_KEYDAT);
  fifo32_put(fifo, data + 512);
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
    mdec->btn = mdec->buf[0] & 0x07;
    mdec->x = (int)mdec->buf[1];
    mdec->y = (int)mdec->buf[2];
    /* make sure large unsigned char is converted to negative int */
    if(mdec->buf[0] & 0x10)
      mdec->x |= 0xffffff00; 
    if(mdec->buf[0] & 0x20)
      mdec->y |= 0xffffff00;
    mdec->y *= -1;
    return 1;
  }
  return -1;
}

