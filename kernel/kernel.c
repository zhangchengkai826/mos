#include "lowlevel.h"
#include "graphic.h"
#include "fonts.h"
#include "keyboard.h"
#include "mouse.h"
#include "stdlib.h"

void init_screen(unsigned char *vram, int xsize, int ysize) {
  boxfill8(vram, xsize, COL8_FF0000, 20, 20, 120, 120);
  putfonts8_asc(vram, xsize, 30, 40, COL8_00FFFF, "hello world!");
}

void init_mouse_cursor8(unsigned char *mouse) {
  int i;
  unsigned char d[] = {224, 240, 248, 254, 255, 255, 255, 252, 252, 220, 28, 14, 14, 6, 7, 3};
  memcpy(mouse, d, 16);
}

void main() {
  struct GATE_DESCRIPTOR idt[256];
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  unsigned char keybuf[32], mousebuf[128];

  io_cli();
  init_idt(idt);
  init_pic();
  set_gatedesc(idt+0x21, (unsigned)asm_inthandler21, 1 << 3, AR_INTGATE32);
  set_gatedesc(idt+0x2c, (unsigned)asm_inthandler2c, 1 << 3, AR_INTGATE32);
  fifo8_init(&keyfifo, 32, keybuf);
  fifo8_init(&mousefifo, 128, mousebuf);
  init_keyboard();
  enable_mouse();
  io_out8(PIC0_IMR, 0xf9);
  io_out8(PIC1_IMR, 0xef);
  io_sti();

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  unsigned char mcursor[16];
  init_mouse_cursor8(mcursor);
  putfont8(binfo->vram, binfo->scrnx, 100, 125, COL8_FFFFFF, mcursor);

  unsigned char i;
  char s[8];
  for(;;) {
    io_cli();
    if(fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
      io_stihlt();
    } else {
      if(fifo8_status(&keyfifo) != 0) {
        i = fifo8_get(&keyfifo);
        io_sti();
        u2s(s, i);
        boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 64, 32);
        putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
      } else if(fifo8_status(&mousefifo) != 0) {
        i = fifo8_get(&mousefifo);
        io_sti();
        u2s(s, i);
        boxfill8(binfo->vram, binfo->scrnx, COL8_848400, 128, 0, 192, 32);
        putfonts8_asc(binfo->vram, binfo->scrnx, 128, 0, COL8_FFFFFF, s);
      }
    }
  }
}

