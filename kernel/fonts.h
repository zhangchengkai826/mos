#ifndef _FONTS_H
#define _FONTS_H

#include "sheet.h"

void putfont8(unsigned char *vram, int xsize, int x, int y, unsigned char c, unsigned char *font);

void putfonts8_asc(char *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);

void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);

#endif /* _FONTS_H */
