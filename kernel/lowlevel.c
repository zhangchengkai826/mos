#include "lowlevel.h"
#include "mem.h"

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

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned limit, int base, int ar) {
  if(limit > 0xfffff) {
    ar |= 0x8000;
    limit /= 0x1000;
  }
  sd->limit_low = limit & 0xffff;
  sd->base_low = base & 0xffff;
  sd->base_mid = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high = (base >> 24) & 0xff;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, unsigned offset, unsigned short selector, unsigned char ar) {
  gd->offset_low = offset & 0xffff;
  gd->selector = selector;
  gd->zero = 0;
  gd->access_right = ar;
  gd->offset_high = (offset >> 16) & 0xffff;
}

void init_gdtidt(struct GATE_DESCRIPTOR *idt) {
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)GDT_ADDR;
  int i;
  for(i = 0; i < 256; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_CODE32_ER);
  set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_DATA32_RW);
  load_gdtr(0x7ff, (unsigned)gdt);
  for(i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(0x7ff, (unsigned)idt);
}


