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

void hlt();
void io_cli();
void io_sti();
uint32 io_load_eflags();
void io_store_eflags(uint32 eflags);
uint8 io_in8(uint16 port);
void io_out8(uint16 port, uint8 data);
void load_idtr(uint16 limit, uint32 addr);

