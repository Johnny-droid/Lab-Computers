#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdlib.h>

extern int global_hook_id;

bool (kbc_communication_error)();
bool (kbc_output_buf_full)();
bool (kbc_input_buf_full)();
int(kbc_subscribe_int)(uint8_t *bit_no);
int(kbc_unsubscribe_int)();
uint8_t (kbc_read_output_buffer)();
void (enable_interrupts)();
void (kbc_ih)(void);


uint8_t (read_command_byte)();
void (write_command_byte)(uint8_t command_byte);
 
 
#endif

