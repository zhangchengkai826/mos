#include "lowlevel.h"
#include "keyboard.h"

#define PORT_KEYDAT 0x0060

struct FIFO8 keyfifo;

void inthandler21(int *esp) {
  unsigned char data;
  io_out8(PIC0_OCW2, 0x61);
  data = io_in8(PORT_KEYDAT);
  fifo8_put(&keyfifo, data);
}

