#ifndef __VIDEOCARD_H
#define __VIDEOCARD_H

#include <machine/int86.h> // /usr/src/include/arch/i386
#include <stdio.h>
#include <stdlib.h>
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include "../game.h"

#include "vg_macros.h"

#include "sprites/alien.h"
#include "sprites/crosshair.h"
#include "sprites/numbers.h"
#include "sprites/buttons.h"
#include "sprites/titles.h"
#include "sprites/letters.h"

#define GAME_FRAME_RATE 30

struct SPRITE {
    char* ptr;
    xpm_image_t info;
};


bool (vg_prepareGraphics)(uint16_t mode);
bool (vg_setGraphics)(uint16_t mode);
bool (vg_free)();
bool (vg_load_sprites)();

int (vg_draw_sprite)(struct SPRITE sprite, uint16_t x, uint16_t y, uint8_t buffer_no);
void (vg_draw_aliens)();
void (vg_draw_crosshair)(uint8_t buffer_no);
void (vg_draw_points)();
void (vg_draw_play_button)();
void (vg_draw_exit_button)();
void (vg_draw_game_name)();
void (vg_draw_paused)();


void (vg_draw_game)();
void (vg_draw_menu)();
void (vg_draw_game_over)();

void (vg_ih)();



#endif



