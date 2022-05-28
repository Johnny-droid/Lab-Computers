#ifndef _GAME_H_
#define _GAME_H_

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include "video_card/video_card.h"
#include "keyboard/keyboard.h"

#define GAME_GRAPHICS_MODE 0x115

//Going to make the states definition here


enum GAME_STATE {
    PLAYING,
    PAUSE, 
    MENU 
};

enum ALIEN_TYPE {
    ALIEN1,
    DEAD, // we could probably add diferent stages of DEAD if we want to create an animation
    EMPTY
};

struct ALIEN {
    enum ALIEN_TYPE type;
};


enum GAME_STATE game_state;


int (game_loop)();


#endif 


