#include "timer.h"
#include "lowlevel.h"
#include "mem.h"
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040
#define TIMER_FLAGS_ALLOC 1
#define TIMER_FLAGS_USING 2

void init_pit() {
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  int i;
  io_out8(PIT_CTRL, 0x34);
  io_out8(PIT_CNT0, 0x9c);
  io_out8(PIT_CNT0, 0x2e);
  timerctl->count = 0;
  timerctl->next = 0xffffffff;
  for(i = 0; i < MAX_TIMER; i++)
    timerctl->timer[i].flags = 0;
}

struct TIMER *timer_alloc() {
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  int i;
  for(i = 0; i < MAX_TIMER; i++) {
    if(timerctl->timer[i].flags == 0) {
      timerctl->timer[i].flags = TIMER_FLAGS_ALLOC;
      return &timerctl->timer[i];
    }
  }
  return 0;
}

void timer_free(struct TIMER *timer) {
  timer->flags = 0;
}

void timer_init(struct TIMER *timer, struct FIFO8 *fifo, unsigned char data) {
  timer->fifo = fifo;
  timer->data = data;
}

void timer_settime(struct TIMER *timer, unsigned timeout) {
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  timer->timeout = timeout + timerctl->count;
  timer->flags = TIMER_FLAGS_USING;
  if(timerctl->next > timer->timeout) {
    timerctl->next = timer->timeout;
  }
}

void inthandler20(int *esp) {
  int i;
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  io_out8(PIC0_OCW2, 0x60);
  timerctl->count++;
  if(timerctl->next > timerctl->count)
    return;
  timerctl->next = 0xffffffff;
  for(i = 0; i < MAX_TIMER; i++) {
    if(timerctl->timer[i].flags == TIMER_FLAGS_USING) {
      if(timerctl->timer[i].timeout <= timerctl->count) {
        timerctl->timer[i].flags = TIMER_FLAGS_ALLOC;
        fifo8_put(timerctl->timer[i].fifo, timerctl->timer[i].data);
      } else {
        if(timerctl->next > timerctl->timer[i].timeout) {
          timerctl->next = timerctl->timer[i].timeout;
        }
      }
    }
  }
}

