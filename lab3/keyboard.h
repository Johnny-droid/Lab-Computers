#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdlib.h>

extern int global_hook_id;

int (kbc_communication_error)();
int (kbc_output_buf_full)();
int(kbc_subscribe_int)(uint8_t *bit_no);
int(kbc_unsubscribe_int)();
uint8_t (kbc_read_output_buffer)();

 

#endif

