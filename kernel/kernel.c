typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned uint32;

#define ADR_BOOTINFO 0x0ff0

#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

#define AR_INTGATE32 0x8e

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

void io_sti() {
  asm("sti");
}

void io_out8(uint16 port, uint8 data);

void init_pic() {
  io_out8(PIC0_IMR, 0xff);
  io_out8(PIC1_IMR, 0xff);

  io_out8(PIC0_ICW1, 0x11);
  io_out8(PIC0_ICW2, 0x20);
  io_out8(PIC0_ICW3, 1 << 2);
  io_out8(PIC0_ICW4, 0x01);

  io_out8(PIC1_ICW1, 0x11);
  io_out8(PIC1_ICW2, 0x28);
  io_out8(PIC1_ICW3, 2);
  io_out8(PIC1_ICW4, 0x01);

  io_out8(PIC0_IMR, 0xfb);
  io_out8(PIC1_IMR, 0xff);
}

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

void memcpy(uint8 *dest, uint8 *src, int size) {
  int i;
  for(i = 0; i < size; i++)
    dest[i] = src[i];
}

void init_mouse_cursor8(uint8 *mouse) {
  int i;
  uint8 d[] = {224, 240, 248, 254, 255, 255, 255, 252, 252, 220, 28, 14, 14, 6, 7, 3};
  memcpy(mouse, d, 16);
}

void putblock8_8(uint8 *vram, int vxsize, int pxsize, int pysize, int px0, int py0, uint8 *buf, int bxsize) {
  int x, y;
  for(y = 0; y < pysize; y++) {
    for(x = 0; x < pxsize; x++) {
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
    }
  }
}

void init_screen(uint8 *vram, int xsize, int ysize) {
  boxfill8(vram, xsize, COL8_FF0000, 20, 20, 120, 120);
  putfonts8_asc(vram, xsize, 30, 40, COL8_00FFFF, "hello world!");
}

void inthandler21(int *esp) {
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;
  putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, "INT 21: PS/2 keyboard");
}

struct GATE_DESCRIPTOR {
  uint16 offset_low, selector;
  uint8 zero, access_right;
  uint16 offset_high;
};

void load_idtr(uint16 limit, uint32 addr);

void set_gatedesc(struct GATE_DESCRIPTOR *gd, uint32 offset, uint16 selector, uint8 ar) {
  gd->offset_low = offset & 0xffff;
  gd->selector = selector;
  gd->zero = 0;
  gd->access_right = ar;
  gd->offset_high = (offset >> 16) & 0xffff;
}

void init_idt(struct GATE_DESCRIPTOR *idt) {
  int i;
  for(i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(0x7ff, (uint32)idt);
}

void asm_inthandler21();

void main() {
  struct GATE_DESCRIPTOR idt[256];
  struct BOOTINFO *binfo = (struct BOOTINFO *)ADR_BOOTINFO;

  io_cli();
  init_idt(idt);
  init_pic();
  set_gatedesc(idt+0x21, (uint32)asm_inthandler21, 1 << 3, AR_INTGATE32);
  io_out8(PIC0_IMR, 0xf9);
  io_sti();

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  uint8 mcursor[16];
  init_mouse_cursor8(mcursor);
  putfont8(binfo->vram, binfo->scrnx, 100, 125, COL8_FFFFFF, mcursor);

  for(;;) hlt();
}

