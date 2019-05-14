#include "lowlevel.h"
#include "graphic.h"
#include "fonts.h"
#include "keyboard.h"
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

  io_cli();
  init_idt(idt);
  init_pic();
  set_gatedesc(idt+0x21, (unsigned)asm_inthandler21, 1 << 3, AR_INTGATE32);
  io_out8(PIC0_IMR, 0xf9);
  io_sti();

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  unsigned char mcursor[16];
  init_mouse_cursor8(mcursor);
  putfont8(binfo->vram, binfo->scrnx, 100, 125, COL8_FFFFFF, mcursor);

  for(;;) {
    hlt();
  }
    
}

