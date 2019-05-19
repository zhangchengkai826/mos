#include "timer.h"
#include "lowlevel.h"
#include "mem.h"
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

void init_pit() {
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  io_out8(PIT_CTRL, 0x34);
  io_out8(PIT_CNT0, 0x9c);
  io_out8(PIT_CNT0, 0x2e);
  timerctl->count = 0;
  timerctl->timeout = 0;
}

void inthandler20(int *esp) {
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  io_out8(PIC0_OCW2, 0x60);
  timerctl->count++;
  if(timerctl->timeout > 0) {
    timerctl->timeout--;
    if(timerctl->timeout == 0) {
      fifo8_put(timerctl->fifo, timerctl->data);
    }
  }
}

void settimer(unsigned timeout, struct FIFO8 *fifo, unsigned char data) {
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  unsigned eflags;
  eflags = io_load_eflags();
  io_cli();
  timerctl->timeout = timeout;
  timerctl->fifo = fifo;
  timerctl->data = data;
  io_store_eflags(eflags);
}

