#ifndef __VIDEOCARD_H
#define __VIDEOCARD_H

#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lcf.h>
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "keyboard2.h"

bool (prepareGraphics)(uint16_t mode);
bool (setGraphics)(uint16_t mode);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

uint32_t (getBlueBitsFirst)(uint32_t first);
uint32_t (getGreenBitsFirst)(uint32_t first);
uint32_t (getRedBitsFirst)(uint32_t first);
uint32_t (calculateColorPatternIndexed)(uint8_t no_rectangles, unsigned int x, unsigned int y, uint32_t first, uint8_t step);
uint32_t (calculateColorPatternDirect)(uint8_t no_rectangles, unsigned int x, unsigned int y, uint32_t first, uint8_t step);

int(kbd_scan)();

#endif



