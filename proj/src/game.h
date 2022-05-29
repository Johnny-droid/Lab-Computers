#ifndef _GAME_H_
#define _GAME_H_

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include "video_card/video_card.h"
#include "keyboard/keyboard.h"

#define GAME_GRAPHICS_MODE 0x115

#define GAME_WIDTH_MATRIX 20
#define GAME_HEIGHT_MATRIX 12

#define GAME_ALIEN_WIDTH 64 // maybe both are not needed because we can get info from xpm_image_t
#define GAME_ALIEN_HEIGHT 64 // when we load sprite. Although we might want a fix value to create margins and stuff in the game

#define NUMBER_ALIEN_STATES 7 // Not counting empty

//Going to make the states definition here


enum GAME_STATE {
    PLAYING,
    PAUSE, 
    MENU 
};

enum ALIEN_STATE {
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
    unsigned int time;
};

enum GAME_STATE game_state;

struct ALIEN game_matrix[GAME_HEIGHT_MATRIX][GAME_WIDTH_MATRIX];

int (game_loop)();
//Probably going to add step or update function


#endif 


