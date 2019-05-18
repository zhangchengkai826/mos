#include "lowlevel.h"
#include "graphic.h"
#include "fonts.h"
#include "keyboard.h"
#include "mouse.h"
#include "mem.h"
#include "sheet.h"
#include "stdlib.h"

#define MEMMAN_ADDR 0x100000

void init_screen8(unsigned char *vram, int xsize, int ysize) {
  boxfill8(vram, xsize, COL8_000000, 0, 0, xsize, ysize);
  putfonts8_asc(vram, xsize, 30, 40, COL8_00FFFF, "hello world!");
}

void init_mouse_cursor8(unsigned char *vram) {
  int i;
  unsigned char d[] = {224, 240, 248, 254, 255, 255, 255, 252, 252, 220, 28, 14, 14, 6, 7, 3};
  boxfill8(vram, 8, COL_TRANSPARENT, 0, 0, 8, 16);
  putfont8(vram, 8, 0, 0, COL8_FFFFFF, d);
}

void make_window8(unsigned char *buf, int xsize, int ysize, char *title) {
  boxfill8(buf, xsize, COL8_C6C6C6, 0, 0, xsize, ysize);
  boxfill8(buf, xsize, COL8_848484, 0, 0, xsize, 20);
  putfonts8_asc(buf, xsize, 2, 2, COL8_FFFFFF, title);
}

void main() {
  struct GATE_DESCRIPTOR idt[256];
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  unsigned char keybuf[32], mousebuf[128];
  struct MOUSE_DEC mdec;
  int mx, my;
  unsigned char i;
  char s[128];
  struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
  unsigned memtotal, memstart;
  struct SHTCTL *shtctl;
  struct SHEET *sht_back, *sht_win, *sht_mouse;
  unsigned char *buf_back, *buf_win, buf_mouse[128];

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

  memstart = (unsigned)MEMMAN_ADDR+sizeof(memman);
  memtotal = memtest(memstart, 0xbfffffff);
  memman_init(memman);
  memman_free(memman, memstart, memtotal-memstart);

  init_palette();

  shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
  sht_back = sheet_alloc(shtctl);
  sht_mouse = sheet_alloc(shtctl);
  sht_win = sheet_alloc(shtctl);

  buf_back = (unsigned char *)memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
  buf_win = (unsigned char *)memman_alloc_4k(memman, 160*68);

  sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, COL_TRANSPARENT);
  sheet_setbuf(sht_mouse, buf_mouse, 8, 16, COL_TRANSPARENT);
  sheet_setbuf(sht_win, buf_win, 160, 68, COL_TRANSPARENT);

  init_screen8(buf_back, binfo->scrnx, binfo->scrny);
  init_mouse_cursor8(buf_mouse);
  make_window8(buf_win, 160, 68, "window");

  sheet_slide(sht_back, 0, 0);
  mx = binfo->scrnx / 2 - 4;
  my = binfo->scrny / 2 - 8;
  sheet_slide(sht_mouse, mx, my);
  sheet_slide(sht_win, 80, 72);

  sheet_updown(sht_back, 0);
  sheet_updown(sht_win, 1);
  sheet_updown(sht_mouse, 2);
  
  sprintf(s, "memory %uKB, free %uKB", memtotal / 1024, memman_total(memman) / 1024);
  putfonts8_asc(buf_back, binfo->scrnx, 0, 150, COL8_FFFF00, s);
  sheet_refresh(sht_back, 0, 150, binfo->scrnx, 166);

  for(;;) {
    io_cli();
    if(fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
      io_stihlt();
    } else {
      if(fifo8_status(&keyfifo) != 0) {
        i = fifo8_get(&keyfifo);
        io_sti();
        sprintf(s, "%u", i);
        boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 64, 32);
        putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s);
        sheet_refresh(sht_back, 0, 0, 64, 32);
      } else if(fifo8_status(&mousefifo) != 0) {
        i = fifo8_get(&mousefifo);
        io_sti();
        if(mouse_decode(&mdec, i) != 0) {
          boxfill8(buf_back, binfo->scrnx, COL8_848400, 128, 0, 300, 32);
          sprintf(s, "%u %u %u", mdec.buf[0], mdec.buf[1], mdec.buf[2]);
          putfonts8_asc(buf_back, binfo->scrnx, 128, 0, COL8_FFFFFF, s);
          sheet_refresh(sht_back, 128, 0, 300, 32);

          mx += mdec.x;
          my += mdec.y;
          if(mx < 0) mx = 0;
          if(my < 0) my = 0;
          if(mx > binfo->scrnx - 1)
            mx = binfo->scrnx - 1;
          if(my > binfo->scrny - 1)
            my = binfo->scrny - 1;
          sheet_slide(sht_mouse, mx, my);
        }
      }
    }
  }
}

