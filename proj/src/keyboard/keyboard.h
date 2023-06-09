#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>
#include <stdlib.h>
#include <stdint.h>
#include "i8042.h"
#include "../game.h"



bool (kbc_communication_error)();
bool (keyboard_check)();
bool (kbc_output_buf_full)();
bool (kbc_input_buf_full)();
int(kbc_subscribe_int)(uint8_t *bit_no);
int(kbc_unsubscribe_int)();
uint8_t (kbc_read_output_buffer)();
void (enable_interrupts)();
uint8_t (read_command_byte)();
void (write_command_byte)(uint8_t command_byte);

/**
 * @brief Handles keyboard interrupts
 * Reads the status register and the output buffer (OB).
 * If there was some error, the byte read from the OB is be discarded.
 */
void (kbc_ih)(void);

/**
 * @brief Handles the events generated by the interrupt handler
 * Decides which action to take according to the game state
 */
void (keyboard_event_handler)(uint8_t scanCode);


 

#endif

