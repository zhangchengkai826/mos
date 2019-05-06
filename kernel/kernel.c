typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned uint32;

#include "fonts.h"

void hlt() {
  asm("hlt");
}

void write_mem8(uint32 addr, uint8 data) {
  *(uint8 *)addr = data;
}

uint32 io_load_eflags();

void io_store_eflags(uint32 eflags);

void io_cli() {
  asm("cli");
}

void io_out8(uint16 port, uint8 data);

void set_palette(int start, int end, uint8 *rgb) {
  uint32 i, eflags;
  eflags = io_load_eflags();
  io_cli();
  io_out8(0x03c8, start);
  
  for(i = start; i <= end; i++) {
    io_out8(0x03c9, rgb[0] / 4u);
    io_out8(0x03c9, rgb[1] / 4u);
    io_out8(0x03c9, rgb[2] / 4u);
    rgb += 3;
  }
  io_store_eflags(eflags); 
}

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void init_palette() {
  uint8 table_rgb[16*3] = {
    0x00, 0x00, 0x00,
    0xff, 0x00, 0x00,
    0x00, 0xff, 0x00,
    0xff, 0xff, 0x00,
    0x00, 0x00, 0xff,
    0xff, 0x00, 0xff,
    0x00, 0xff, 0xff,
    0xff, 0xff, 0xff,
    0xc6, 0xc6, 0xc6,
    0x84, 0x00, 0x00,
    0x00, 0x84, 0x00,
    0x84, 0x84, 0x00,
    0x00, 0x00, 0x84,
    0x84, 0x00, 0x84,
    0x00, 0x84, 0x84,
    0x84, 0x84, 0x84,
  };
  set_palette(0, 15, table_rgb);
}

void boxfill8(uint8 *vram, int xsize, uint8 c, int x0, int y0, int x1, int y1) {
  int x, y;
  for(y = y0; y <= y1; y++)
    for(x = x0; x <= x1; x++)
      vram[y * xsize + x] = c;
}

struct BOOTINFO {
  uint8 cyls, leds, vmode, reserve;
  uint16 scrnx, scrny;
  uint8 *vram;
};

void init_screen(uint8 *vram, int xsize, int ysize) {
  boxfill8(vram, xsize, COL8_FF0000, 20, 20, 120, 120);
  putfonts8_asc(vram, xsize, 30, 40, COL8_00FFFF, "hello world!");
}

void main() {
  struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  hlt();
}

