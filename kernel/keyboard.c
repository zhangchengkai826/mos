#include "lowlevel.h"
#include "keyboard.h"
#include "mem.h"

#define KEYSTA_SEND_NOTREADY 0x02
#define KBC_MODE 0x47

void inthandler21(int *esp) {
  int data;
  struct FIFO32 *fifo = (struct FIFO32 *)FIFO_ADDR;
  io_out8(PIC0_OCW2, 0x61);
  data = io_in8(PORT_KEYDAT);
  fifo32_put(fifo, data + 256);
}

void wait_KBC_sendready() {
  for(;;)
    if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0)
      break;
}

void init_keyboard() {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, KBC_MODE);
}

