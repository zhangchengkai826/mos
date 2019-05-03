typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned uint32;

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

void main() {
  uint32 i;
  init_palette();
  for(i = 0xa0000u; i <= 0xaffffu; i++) {
    write_mem8(i, i & 0xfu);
  }
  hlt();
}

