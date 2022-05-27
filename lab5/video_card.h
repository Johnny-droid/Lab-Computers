#ifndef __VIDEOCARD_H
#define __VIDEOCARD_H

#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lcf.h>
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "keyboard2.h"
#include <lcom/timer.h>

bool (prepareGraphics)(uint16_t mode);
bool (setGraphics)(uint16_t mode);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_sprite)(char* sprite, uint16_t x, uint16_t y, uint8_t buffer_no, xpm_image_t img_info);
int (vg_draw_moving_sprite)(char* sprite, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate, xpm_image_t img_info);

uint32_t (getBlueBitsFirst)(uint32_t first);
uint32_t (getGreenBitsFirst)(uint32_t first);
uint32_t (getRedBitsFirst)(uint32_t first);
uint32_t (calculateColorPatternIndexed)(uint8_t no_rectangles, unsigned int x, unsigned int y, uint32_t first, uint8_t step);
uint32_t (calculateColorPatternDirect)(uint8_t no_rectangles, unsigned int x, unsigned int y, uint32_t first, uint8_t step);

int(kbd_scan)();

#endif


