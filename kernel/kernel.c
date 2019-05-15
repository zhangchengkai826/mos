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
  struct MOUSE_DEC mdec;
  int mx, my;
  unsigned char mcursor[16];
  unsigned char i;
  unsigned memsz;
  char s[8];

  io_cli();
  init_idt(idt);
  init_pic();
  set_gatedesc(idt+0x21, (unsigned)asm_inthandler21, 1 << 3, AR_INTGATE32);
  set_gatedesc(idt+0x2c, (unsigned)asm_inthandler2c, 1 << 3, AR_INTGATE32);
  fifo8_init(&keyfifo, 32, keybuf);
  fifo8_init(&mousefifo, 128, mousebuf);
  init_keyboard();
  enable_mouse(&mdec);
  io_out8(PIC0_IMR, 0xf9);
  io_out8(PIC1_IMR, 0xef);
  io_sti();

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  init_mouse_cursor8(mcursor);
  mx = binfo->scrnx / 2 - 4;
  my = binfo->scrny / 2 - 8;
  putfont8(binfo->vram, binfo->scrnx, mx, my, COL8_FFFFFF, mcursor);

  memsz = memtest(0x00002000, 0xbfffffff);
  u2s(s, memsz);
  putfonts8_asc(binfo->vram, binfo->scrnx, 0, 50, COL8_FFFFFF, s);

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
        if(mouse_decode(&mdec, i) != 0) {
          boxfill8(binfo->vram, binfo->scrnx, COL8_848400, 128, 0, 300, 32);
          u2s(s, mdec.buf[0]);
          putfonts8_asc(binfo->vram, binfo->scrnx, 130, 0, COL8_FFFFFF, s);
          u2s(s, mdec.buf[1]);
          putfonts8_asc(binfo->vram, binfo->scrnx, 200, 0, COL8_FFFFFF, s);
          u2s(s, mdec.buf[2]);
          putfonts8_asc(binfo->vram, binfo->scrnx, 270, 0, COL8_FFFFFF, s);
          init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
          mx += mdec.x;
          my += mdec.y;
          if(mx < 0) mx = 0;
          if(my < 0) my = 0;
          if(mx > binfo->scrnx - 8)
            mx = binfo->scrnx - 8;
          if(my > binfo->scrny - 16)
            my = binfo->scrny - 16;
          putfont8(binfo->vram, binfo->scrnx, mx, my, COL8_FFFFFF, mcursor);
        }
      }
    }
  }
}

