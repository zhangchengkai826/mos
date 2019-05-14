#ifndef _FONTS_H
#define _FONTS_H

void putfont8(unsigned char *vram, int xsize, int x, int y, unsigned char c, unsigned char *font);

void putfonts8_asc(char *vram, int xsize, int x, int y, unsigned char c, unsigned char *s);

#endif /* _FONTS_H */
