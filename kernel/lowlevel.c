#include "lowlevel.h"

void hlt() {
  asm("hlt");
}

void io_cli() {
  asm("cli");
}

void io_sti() {
  asm("sti");
}

void io_stihlt() {
  asm("sti");
  asm("hlt");
}

void write_mem8(unsigned addr, unsigned char data) {
  *(unsigned char *)addr = data;
}

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

void set_gatedesc(struct GATE_DESCRIPTOR *gd, unsigned offset, unsigned short selector, unsigned char ar) {
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
  load_idtr(0x7ff, (unsigned)idt);
}

#define CR0_CACHE_DISABLE 0x60000000
unsigned memtest_sub(unsigned start, unsigned end);
unsigned memtest(unsigned start, unsigned end) {
  unsigned cr0, i;
  cr0 = load_cr0();
  cr0 |= CR0_CACHE_DISABLE;
  store_cr0(cr0);

  i = memtest_sub(start, end);

  cr0 = load_cr0();
  cr0 &= ~CR0_CACHE_DISABLE;
  store_cr0(cr0);
  return i;
}


