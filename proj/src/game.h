#ifndef _GAME_H_
#define _GAME_H_

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <time.h>
#include <stdlib.h>
#include "video_card/video_card.h"
#include "keyboard/keyboard.h"

#define GAME_GRAPHICS_MODE 0x115
#define GAME_WIDTH 800
#define GAME_HEIGHT 600


// LAYOUT
#define GAME_HORIZONTAL_MARGIN 15 //total 30 both sides
#define GAME_VERTICAL_MARGIN 20   //total 40 both sides

#define GAME_WIDTH_MATRIX 11
#define GAME_HEIGHT_MATRIX 8

#define ALIEN_WIDTH 70 // maybe both are not needed because we can get info from xpm_image_t
#define ALIEN_HEIGHT 70 // when we load sprite. Although we might want a fix value to create margins and stuff in the game

#define ALIEN_HORIZONTAL_MARGIN 3 // (70 - 64) / 2
#define ALIEN_VERTICAL_MARGIN 3   // (70 - 64) / 2


// GAMEPLAY 
#define NUMBER_ALIEN_STATES 7 // Not counting empty

#define ALIEN_INIT_SPAWN_TIME 70        // in frames
#define ALIEN_SPAWN_RATE_INCREASE  10   // in frames
#define ALIEN_RANDOM_SPAWN_ATTEMPTS 3  
#define ALIEN_MAX_SPAWN_RATE 5          // in frames

enum GAME_STATE { // We can probably add a game over state as well
    PLAYING,
    PAUSE, 
    MENU,
    GAME_OVER,
};

enum ALIEN_STATE { // If changed, don't forget to change the macro above
    APPEARING,
    ALIVE,
    DEAD_1, //The dead states are used for animation
    DEAD_2,
    DEAD_3,
    DEAD_4,
    DEAD_5,
    EMPTY
};

struct ALIEN {
    // if we want other aliens we can just add a type here
    enum ALIEN_STATE state;
    unsigned int time;      // TIME LEFT FOR
};

enum GAME_STATE game_state;

struct ALIEN game_matrix[GAME_HEIGHT_MATRIX][GAME_WIDTH_MATRIX];

static unsigned int alien_times[NUMBER_ALIEN_STATES + 1] = {
    10,     // APPEARING                       
    360,    // ALIVE
    20,     // DEAD_1
    20,     // DEAD_2
    20,     // DEAD_3
    20,     // DEAD_4
    20,     // DEAD_5
    0       // EMPTY
};

void (game_initialize)();
int (game_loop)();
void (game_ih)();

void (game_step)();
void (game_update_alien_times)();
void (game_generate_new_alien)();

#endif 


