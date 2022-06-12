#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdlib.h>
#include <stdint.h>
#include "../game.h"
#include "keyboard/keyboard.h"
#include "ps2.h"

struct MOUSE_EVENT {
  int16_t moveX;
  int16_t moveY;
  bool lbdown;  //rbdown, mbdown;
  bool overflow;
};

int(mouse_subscribe_int)(uint8_t *bit_no);
int(mouse_unsubscribe_int)();
int (read_ACK_byte)();
int (mouse_disable_data_reporting)(int mode);
int(_mouse_enable_data_reporting_)();


/**
 * @brief Mouse interrupt handler
 */
void (mouse_ih)();

/**
 * @brief Mouse event handler.
 * It makes the changes required according to event that it receives
 */
void (mouse_event_handler)(struct MOUSE_EVENT mouse_event);

/**
 * @brief Check for an alien kill
 */
void (mouse_check_kill)();

/**
 * @brief Checks if the play button is pressed
 */
void (mouse_check_play_button)();

/**
 * @brief Checks if the exit button is pressed
 * 
 */
void (mouse_check_exit_button)();





 

#endif
