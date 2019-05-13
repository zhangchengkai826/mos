struct KEYBUF {
  unsigned char data[32];
  int next_r, next_w, len;
};

extern struct KEYBUF keybuf;

void asm_inthandler21();
void inthandler21(int *esp);

