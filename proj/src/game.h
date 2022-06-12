#ifndef _GAME_H_
#define _GAME_H_

#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <time.h>
#include <stdlib.h>
#include "video_card/video_card.h"
#include "keyboard/keyboard.h"
#include "mouse/mouse.h"
#include "leaderboard/leaderboard.h"
#include "rtc/rtc.h"

#define GAME_GRAPHICS_MODE 0x115
#define GAME_WIDTH 800
#define GAME_HEIGHT 600


// LAYOUT

#define GAME_HORIZONTAL_MARGIN 15 //total 30 both sides
#define GAME_VERTICAL_MARGIN 95   

#define GAME_WIDTH_MATRIX 11
#define GAME_HEIGHT_MATRIX 7

#define ALIEN_WIDTH 70 // maybe both are not needed because we can get info from xpm_image_t
#define ALIEN_HEIGHT 70 // when we load sprite. Although we might want a fix value to create margins and stuff in the game

#define ALIEN_HORIZONTAL_MARGIN 3 // (70 - 64) / 2
#define ALIEN_VERTICAL_MARGIN 3   // (70 - 64) / 2

#define CROSSHAIR_WIDTH 64
#define CROSSHAIR_HEIGHT 64

#define POINTS_WIDTH_MARGIN  700
#define POINTS_HEIGHT_MARGIN 32
#define POINTS_BETWEEN_MARGIN 5

#define BUTTON_MARGIN_TOP 100
#define GAME_MARGIN_TOP 150


// GAMEPLAY 
#define NUMBER_ALIEN_STATES 7 // Not counting empty

#define ALIEN_INIT_SPAWN_TIME 70        // in frames
#define ALIEN_SPAWN_RATE_INCREASE  5   // in frames
#define ALIEN_RANDOM_SPAWN_ATTEMPTS 2  
#define ALIEN_MAX_SPAWN_RATE 20          // in frames

#define MOUSE_INIT_X (GAME_WIDTH / 2)
#define MOUSE_INIT_Y (GAME_HEIGHT / 2)

#define GAME_OVER_WAIT 120

uint32_t game_over_counter;

enum GAME_STATE {
    PLAYING,
    PAUSE, 
    MENU,
    GAME_OVER,
    EXIT,
    LEADERBOARD,
    LB_INPUT
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
    enum ALIEN_STATE state;
    unsigned int time;      // TIME LEFT FOR
};

struct KILLER_ALIEN {
    size_t state;
    size_t x;
    size_t y;
};

struct KILLER_ALIEN killer_alien;


enum GAME_STATE game_state;
struct ALIEN game_matrix[GAME_HEIGHT_MATRIX][GAME_WIDTH_MATRIX];
unsigned int points;

static unsigned int alien_times[NUMBER_ALIEN_STATES + 1] = {
    5,     // APPEARING                       
    300,    // ALIVE
    2,     // DEAD_1
    2,     // DEAD_2
    2,     // DEAD_3
    2,     // DEAD_4
    2,     // DEAD_5
    0       // EMPTY
};

// MOUSE VARIABLES
int16_t mouse_x;
int16_t mouse_y;
int16_t crosshair_half_width; 
int16_t crosshair_half_height;  
int16_t crosshair_width_border;
int16_t crosshair_height_border;

// BUTTON/TITLES VARIABLES
uint16_t play_button_xi;
uint16_t play_button_yi;
uint16_t play_button_xf;
uint16_t play_button_yf;

uint16_t exit_button_xi;
uint16_t exit_button_yi;
uint16_t exit_button_xf;
uint16_t exit_button_yf;

char name[7];
char input_message[100];

struct leaderboard LB;

uint8_t minutes;
uint8_t hours;


/**
 * @brief Initializes the game by setting 
 * the variables necessary for game to run 
 */
void (game_initialize)();

/**
 * @brief The game loop of the game
 * Subscribes to the interrutps required, 
 * Enters the loop cycle where the handlers are called
 * And finally it unsubscribes when the game is over
 */
int (game_loop)();

/**
 * @brief Interrupt handler for the game,
 * it updates the game model, according to 
 * the current game state
 */
void (game_ih)();


/**
 * @brief Updates the game model while playing
 */
void (game_step)();


/**
 * @brief Clears the game matrix with "Empty" state aliens
 */
void (game_reset)();

/**
 * @brief Updates the variable time of all aliens
 * and checks special conditions like the players death
 * and changes the alien states at the right time
 */
void (game_update_alien_times)();

/**
 * @brief Handles the spawn rate of 
 * aliens and generates new aliens
 */
void (game_generate_new_alien)();

/**
 * @brief Kills all the aliens except
 *  the one that killed the player
 */
void (kill_other_aliens)();


/**
 * @brief Prepares the variables required
 * and changes the game state to leaderboard
 */
void (game_leaderboard)();

/**
 * @brief Saves and displays the leaderboard
 */
void (game_save_and_display_lb)();


#endif 


