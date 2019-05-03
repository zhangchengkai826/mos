typedef unsigned char uint8;
typedef unsigned uint32;

void write_mem8(uint32 addr, uint8 data) {
  *(uint8 *)addr = data;
}

void main() {
  uint32 i;
  for(i = 0xa0000u; i <= 0xaffffu; i++) {
    write_mem8(i, 15u);
  }
}

