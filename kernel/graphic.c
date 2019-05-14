#include "graphic.h"
#include "lowlevel.h"

void set_palette(int start, int end, unsigned char *rgb) {
  unsigned i, eflags;
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

void init_palette() {
  unsigned char table_rgb[16*3] = {
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

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1) {
  int x, y;
  for(y = y0; y <= y1; y++)
    for(x = x0; x <= x1; x++)
      vram[y * xsize + x] = c;
}

void putblock8_8(unsigned char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, unsigned char *buf, int bxsize) {
  int x, y;
  for(y = 0; y < pysize; y++) {
    for(x = 0; x < pxsize; x++) {
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
    }
  }
}

