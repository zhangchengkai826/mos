#include "lowlevel.h"
#include "graphic.h"
#include "fonts.h"
#include "keyboard.h"
#include "mouse.h"
#include "mem.h"
#include "sheet.h"
#include "timer.h"
#include "stdlib.h"

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
  boxfill8(buf, xsize, COL8_FFFFFF, 0, 0, xsize, ysize);
  boxfill8(buf, xsize, COL8_848484, 0, 0, xsize, 20);
  putfonts8_asc(buf, xsize, 2, 2, COL8_FFFFFF, title);
}

void main() {
  struct GATE_DESCRIPTOR idt[256];
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  struct FIFO32 *fifo = (struct FIFO32 *)FIFO_ADDR;
  int fifobuf[128];
  struct TIMERCTL *timerctl = (struct TIMERCTL *)TIMERCTL_ADDR;
  struct MOUSE_DEC mdec;
  int mx, my;
  int i;
  char s[128];
  struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
  unsigned memtotal, memstart;
  struct SHTCTL *shtctl;
  struct SHEET *sht_back, *sht_win, *sht_mouse;
  unsigned char *buf_back, *buf_win, buf_mouse[128];
  struct TIMER *timer, *timer2, *timer3;
  int cursor_x, cursor_c;
  char keytable[] = {0, 0, '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', '-', '^', 0, 0, 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '@', '[', 0, 0, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', ':', 0, 0, ']', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6',
     '+', '1', '2', '3', '0', '.'
  };
  io_cli();
  init_idt(idt);
  init_pic();
  init_pit();
  set_gatedesc(idt+0x20, (unsigned)asm_inthandler20, 1 << 3, AR_INTGATE32);
  set_gatedesc(idt+0x21, (unsigned)asm_inthandler21, 1 << 3, AR_INTGATE32);
  set_gatedesc(idt+0x2c, (unsigned)asm_inthandler2c, 1 << 3, AR_INTGATE32);
  fifo32_init(fifo, 128, fifobuf);
  init_keyboard();
  enable_mouse(&mdec);
  io_out8(PIC0_IMR, 0xf8);
  io_out8(PIC1_IMR, 0xef);
  io_sti();

  memstart = ((unsigned)MEMMAN_ADDR+sizeof(struct MEMMAN)+0xfff)&0xfffff000;
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
  sheet_setbuf(sht_win, buf_win, 320, 240, COL_TRANSPARENT);

  init_screen8(buf_back, binfo->scrnx, binfo->scrny);
  init_mouse_cursor8(buf_mouse);
  make_window8(buf_win, 320, 240, "My 1st window");

  sheet_slide(sht_back, 0, 0);
  mx = binfo->scrnx / 2 - 4;
  my = binfo->scrny / 2 - 8;
  sheet_slide(sht_mouse, mx, my);
  sheet_slide(sht_win, 320, 240);

  sheet_updown(sht_back, 0);
  sheet_updown(sht_win, 1);
  sheet_updown(sht_mouse, 2);
  
  sprintf(s, "memory %uKB, free %uKB", memtotal / 1024, memman_total(memman) / 1024);
  putfonts8_asc_sht(sht_back, 0, 150, COL8_FFFF00, COL8_FF0000, s, strlen(s));
  /* debug start 
  sprintf(s, "[1]a:%u, s:%u", MOUSEFIFO_ADDR, (unsigned)mousefifo);
  putfonts8_asc(buf_back, binfo->scrnx, 0, 166, COL8_FFFF00, s);
  unsigned atmp = memman_alloc_4k(memman, 1);
  sprintf(s, "[2]a:%u, t:%u", memman->free[0].addr, atmp);
  putfonts8_asc(buf_back, binfo->scrnx, 0, 182, COL8_FFFF00, s);
  debug end */

  timer = timer_alloc();
  timer_init(timer, fifo, 10);
  timer_settime(timer, 10000);
  timer2 = timer_alloc();
  timer_init(timer2, fifo, 3);
  timer_settime(timer2, 5000);
  timer3 = timer_alloc();
  timer_init(timer3, fifo, 1);
  timer_settime(timer3, 500);

  for(;;) {
    sprintf(s, "%u", timerctl->count);
    putfonts8_asc_sht(sht_win, 40, 46, COL8_000000, COL8_C6C6C6, s, strlen(s));

    io_cli();
    if(fifo32_status(fifo) == 0) {
      io_sti();
    } else {
      i = fifo32_get(fifo);
      io_sti();
      if(i >= 256 && i < 512) { /* keyboard */
        sprintf(s, "%u", i-256);
        putfonts8_asc_sht(sht_back, 0, 0, COL8_000000, COL8_008484, s, strlen(s));
        if(i-256 < 0x54) {
          if(keytable[i-256] != 0 && cursor_x < 312) {
            s[0] = keytable[i-256];
            s[1] = 0;
            putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, s, 1);
            cursor_x += 10;
          }
        }
        if(i-256 == 0x0e && cursor_x > 8) {
          putfonts8_asc_sht(sht_win, cursor_x, 28, COL8_000000, COL8_FFFFFF, " ", 1);
          cursor_x -= 10;
        }
        boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x+8, 44);
        sheet_refresh(sht_win, cursor_x, 28, cursor_x+8, 44);
      } else if(i >= 512 && i < 768) { /* mouse */
        if(mouse_decode(&mdec, i-512) != 0) {
          sprintf(s, "%u %u %u", mdec.buf[0], mdec.buf[1], mdec.buf[2]);
          putfonts8_asc_sht(sht_back, 128, 0, COL8_000000, COL8_848400, s, 12);

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
      } else if(i == 10) {
        putfonts8_asc_sht(sht_back, 0, 64, COL8_FFFFFF, COL8_840084, "10000 ticks", 11);
      } else if(i == 3) {
        putfonts8_asc_sht(sht_back, 0, 80, COL8_FFFFFF, COL8_840084, "5000 ticks", 10);
      } else if(i <= 1) {
        if(i != 0) {
          timer_init(timer3, fifo, 0);
          cursor_c = COL8_000000;
        }
        else if(i == 0) {
          timer_init(timer3, fifo, 1);
          cursor_c = COL8_FFFFFF;
        }
        timer_settime(timer3, 500);
        boxfill8(sht_win->buf, sht_win->bxsize, cursor_c, cursor_x, 28, cursor_x+8, 44);
        sheet_refresh(sht_win, cursor_x, 28, cursor_x+8, 44);
      }
    } 
  }
}


