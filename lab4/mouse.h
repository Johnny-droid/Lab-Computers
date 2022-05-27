#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdlib.h>
#include <stdint.h>
#include "ps2.h"

#define MIN_SLOPE 1.0

typedef enum { 
  INIT, DRAW_UP, DRAW_DOWN, DETECTED 
} state_t;
typedef enum {
  RBDOWN, RBUP, LBDOWN, LBUP, MBDOWN,MBUP, MOVE 
} event_type_t;
typedef struct {
  event_type_t type;
  uint8_t moveX;
  uint8_t moveY;
  bool lbdown, rbdown, mbdown;
} mouse_event_t;

extern int global_hook_id;

bool (kbc_communication_error)();
bool (kbc_output_buf_full)();
bool (kbc_input_buf_full)();
int(mouse_subscribe_int)(uint8_t *bit_no);
int(mouse_unsubscribe_int)();
uint8_t (kbc_read_output_buffer)();
bool (mouse_check)();
void (mouse_ih)();
int (read_ACK_byte)();
int (mouse_disable_data_reporting)(int mode);
void (mouse_check_pattern)(mouse_event_t evt);


 

#endif
