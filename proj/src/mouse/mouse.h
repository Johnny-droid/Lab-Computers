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


void (mouse_ih)();
void (mouse_event_handler)(struct MOUSE_EVENT mouse_event);
void (mouse_check_kill)();
void (mouse_check_play_button)();
int (read_ACK_byte)();
int (mouse_disable_data_reporting)(int mode);



 

#endif
