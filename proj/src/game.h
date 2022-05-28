#ifndef _GAME_H_
#define _GAME_H_

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include "keyboard/keyboard.h"

#define GAME_GRAPHICS_MODE 0x115
#define GAME_FRAME_RATE 30


enum GAME_STATE {
    PLAYING,
    PAUSE, 
    MENU // Maybe
};



//Probably going to make the states definition here

int (game_loop)();


#endif 


