#ifndef _STDLIB_H
#define _STDLIB_H

/* fifo.c */
struct FIFO8 {
  unsigned char *buf;
  int p, q, size, free, flags;
};
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

/* stdlib.c */
int u2s(char buf[], unsigned val);
void memcpy(unsigned char *dest, unsigned char *src, int size);
int strlen(const char *s);
void sprintf(char *buf, const char *format, ...);

#endif /* _STDLIB_H */
