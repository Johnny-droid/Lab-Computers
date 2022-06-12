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

char str[256];

struct SPRITE {
    char* ptr;
    xpm_image_t info;
};


/**
 * @brief Makes required steps to draw in the video_card
 * Stores in the global variables the values required 
 * and maps the video memory
 */
bool (vg_prepareGraphics)(uint16_t mode);

/**
 * @brief Changes to the graphics mode given
 */
bool (vg_setGraphics)(uint16_t mode);

/**
 * @brief Frees allocated memory of the buffer
 */
bool (vg_free)();

/**
 * @brief Loads all the sprites required for the game
 */
bool (vg_load_sprites)();

/**
 * @brief Draws normal sprites
 */
int (vg_draw_sprite)(struct SPRITE sprite, uint16_t x, uint16_t y, uint8_t buffer_no);

/**
 * @brief Draws the aliens in the matrix
 */
void (vg_draw_aliens)();

/**
 * @brief Draws the aliens that killed the player
 * Special overlapping attetion is required
 */
void (vg_draw_killer_alien)();

/**
 * @brief Draws the crosshair
 * Special overlapping attetion is required
 */
void (vg_draw_crosshair)(uint8_t buffer_no);

/**
 * @brief Draws the points in the top right
 * corner of the playing game screen
 */
void (vg_draw_points)();

/**
 * @brief Draws the play button (pressed or unpressed)
 */
void (vg_draw_play_button)();

/**
 * @brief Draws the exit button (pressed or unpressed)
 */
void (vg_draw_exit_button)();

/**
 * @brief Draws the game title
 */
void (vg_draw_game_name)();

/**
 * @brief Draws the paused state
 */
void (vg_draw_paused)();

/**
 * @brief Draws the string given in the x0 and y0 values
 */
void (vg_draw_str)(char * str, int x, int y);

/**
 * @brief Draws the game plaing state
 */
void (vg_draw_game)();

/**
 * @brief Draws the menu
 */
void (vg_draw_menu)();


/**
 * @brief Draws the game over
 */
void (vg_draw_game_over)();

/**
 * @brief Draws the input username screen
 */
void (vg_draw_input_screen)();

/**
 * @brief Draws the leaderboard screen
 */
void (vg_draw_leadeboard)();


/**
 * @brief Interrupt handler of the video card. 
 * It's responsible for calling the right drawing 
 * function according to the game state
 */
void (vg_ih)();

/**
 * @brief Draws the time in the menu
 */
void (vg_draw_time_info)() ;


#endif



