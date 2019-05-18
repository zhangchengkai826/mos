#include "lowlevel.h"
#include "keyboard.h"
#include "mem.h"

#define KEYSTA_SEND_NOTREADY 0x02
#define KBC_MODE 0x47

struct FIFO8 *keyfifo = (struct FIFO8 *)KEYFIFO_ADDR;

void inthandler21(int *esp) {
  unsigned char data;
  io_out8(PIC0_OCW2, 0x61);
  data = io_in8(PORT_KEYDAT);
  fifo8_put(keyfifo, data);
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

