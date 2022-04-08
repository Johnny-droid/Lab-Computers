#include <lcom/lcf.h>

#include <stdint.h>
#include "i8042.h"

int global_hook_id;
bool flag_cycle_ESC;
int g_counter;

/** Prints the input scancode. (Already implemented in the LCF)
 * Parameters
 * make	Whether this is a make or a break code
 * size	Size in bytes of the scancode
 * bytes	Array with size elements, with the scancode bytes
 */
//int kbd_print_scancode(bool make, uint8_t size, uint8_t * bytes )	


// Prints the no. of sys_inb() calls (Already implemented in the LCF): cnt Number of sys_inb() calls
// int kbd_print_no_sysinb	(uint32_t cnt)	

int (kbc_communication_error)() {
  uint8_t status_reg = 0;
  util_sys_inb(KBC_STAT_REG, &status_reg);
  //printf("Status reg inside comm error: %x\n ", status_reg & KBC_STAT_CHECK);
  if ((status_reg & KBC_STAT_CHECK) == OK) return 0; 
  return 1;
}

int (kbc_output_buf_full)() {
  uint8_t status_reg = 0;
  util_sys_inb(KBC_STAT_REG, &status_reg);
  if (status_reg & KBC_OUT_BUF_FULL) return 0; 
  return 1;
}

uint8_t (kbc_read_output_buffer)(){
  uint8_t scan_code;
  util_sys_inb(KBC_OUT_BUF, &scan_code);
  return scan_code;
}



/**
 * Handles keyboard interrupts (C implementation)
 * Reads the status register and the output buffer (OB).
 * If there was some error, the byte read from the OB should be discarded.
 * All communication with other code must be done via global variables, static if possible.
 */
void (kbc_ih)(void) {

  if (kbc_output_buf_full() != OK) return;

  g_counter++;
  uint8_t scan_code, size = 1;
  uint8_t* arr = (uint8_t*) malloc(2 * sizeof(uint8_t));
  scan_code=kbc_read_output_buffer();

  if (scan_code == KBC_SCANCODE_2B) {
    arr[1] = scan_code;
    size++;
    while (scan_code == KBC_SCANCODE_2B ) {
      scan_code = kbc_read_output_buffer(); 
    }
  }

  arr[0] = scan_code;

  if (kbc_communication_error() != OK) return;

  if (scan_code == ESC_BREAK) flag_cycle_ESC = false;

  kbd_print_scancode(!(scan_code & BIT(7)),size,arr);
  free(arr);
}


int (kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBC_IRQ;
  int res = sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &global_hook_id);
  return res;
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&global_hook_id);
}

