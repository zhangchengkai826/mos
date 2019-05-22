#ifndef _LOWLEVEL_H
#define _LOWLEVEL_H

#define ADR_BOOTINFO 0x0ff0

#define AR_CODE32_ER 0x409a
#define AR_DATA32_RW 0x4092
#define AR_TSS32 0x0089

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

struct SEGMENT_DESCRIPTOR {
  short limit_low, base_low;
  char base_mid, access_right;
  char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
  unsigned short offset_low, selector;
  unsigned char zero, access_right;
  unsigned short offset_high;
};

void hlt();
void io_cli();
void io_sti();
void io_stihlt();
unsigned io_load_eflags();
void io_store_eflags(unsigned eflags);
int io_in8(unsigned short port);
int io_in32(unsigned short port);
void io_out8(unsigned short port, unsigned char data);
void load_gdtr(unsigned short limit, unsigned addr);
void load_idtr(unsigned short limit, unsigned addr);
void write_mem8(unsigned addr, unsigned char data);
void init_pic();
void set_gatedesc(struct GATE_DESCRIPTOR *gd, unsigned offset, unsigned short selector, unsigned char ar);
void init_gdtidt(struct SEGMENT_DESCRIPTOR *gdt, struct GATE_DESCRIPTOR *idt);
unsigned load_cr0();
void store_cr0(unsigned cr0);

#endif /* _LOWLEVEL_H */
