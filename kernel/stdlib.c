#include "stdlib.h"

int u2s(char buf[], unsigned val) {
  int i, k;
  unsigned remainder;
  char c;
  i = 0;
  while(1) {
    remainder = val % 10;
    c = (char)remainder + 48;
    buf[i] = c;
    i++;
    val /= 10;
    if(val == 0) break;
  }
  for(k = 0; k < i/2; k++) {
    c = buf[k];
    buf[k] = buf[i-k-1];
    buf[i-k-1] = c;
  }
  buf[i] = '\0';
  return i;
}

