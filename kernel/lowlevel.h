#ifndef _LOWLEVEL_H
#define _LOWLEVEL_H

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

struct BOOTINFO {
  unsigned char cyls, leds, vmode, reserve;
  unsigned short scrnx, scrny;
  unsigned char *vram;
};

struct GATE_DESCRIPTOR {
  unsigned short offset_low, selector;
  unsigned char zero, access_right;
  unsigned short offset_high;
};

void hlt();
void io_cli();
void io_sti();
unsigned io_load_eflags();
void io_store_eflags(unsigned eflags);
unsigned char io_in8(unsigned short port);
void io_out8(unsigned short port, unsigned char data);
void load_idtr(unsigned short limit, unsigned addr);
void write_mem8(unsigned addr, unsigned char data);
void init_pic();
void set_gatedesc(struct GATE_DESCRIPTOR *gd, unsigned offset, unsigned short selector, unsigned char ar);
void init_idt(struct GATE_DESCRIPTOR *idt);

#endif /* _LOWLEVEL_H */
