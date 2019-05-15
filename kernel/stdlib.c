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

void memcpy(unsigned char *dest, unsigned char *src, int size) {
  int i;
  for(i = 0; i < size; i++)
    dest[i] = src[i];
}

int strlen(const char *s) {
  const char *p = s;
  int n = 0;
  while(*p) {
    n++;
    p++;
  }
  return n;
}

void sprintf(char *buf, const char *format, ...) {
  const char *p = format;
  char *q = buf;
  unsigned char *a = (unsigned char *)&format;
  while(*p) {
    if(*p == '%') {
      ++p;
      if(*p == 'u') {
        a += 4;
        q += u2s(q, *(unsigned *)a);
        ++p;
        continue;
      }
      --p;
    }
    *q = *p;
    p++;
    q++;
  }
  *q = '\0';
}

