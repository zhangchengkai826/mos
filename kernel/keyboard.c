#include "lowlevel.h"
#include "keyboard.h"

#define PIC0_OCW2 0x0020
#define PORT_KEYDAT 0x0060

struct KEYBUF keybuf;

void inthandler21(int *esp) {
  unsigned char data, s[4];
  io_out8(PIC0_OCW2, 0x61);
  data = io_in8(PORT_KEYDAT);
  if(keybuf.len < 32) {
    keybuf.data[keybuf.next_w] = data;
    keybuf.len++;
    keybuf.next_w++;
    if(keybuf.next_w == 32) {
      keybuf.next_w = 0;
    }
  }
}

