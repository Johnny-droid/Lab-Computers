#include <lcom/lcf.h>

#include <stdint.h>
#include "i8042.h"

int kbc_hook_id = 1;
bool flagESC;
bool flag2Bytes;
int counter_sys_in;


/** Prints the input scancode. (Already implemented in the LCF)
 * Parameters
 * make	Whether this is a make or a break code
 * size	Size in bytes of the scancode
 * bytes	Array with size elements, with the scancode bytes
 */
//int kbd_print_scancode(bool make, uint8_t size, uint8_t * bytes )	


// Prints the no. of sys_inb() calls (Already implemented in the LCF): cnt Number of sys_inb() calls
// int kbd_print_no_sysinb	(uint32_t cnt)	

bool (kbc_communication_error)() {
  uint8_t st = 0;
  counter_sys_in++;
  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ( (st & KBC_STAT_CHECK) == 0) return false;
  return true;

}

bool (kbc_output_buf_full)() {
  uint8_t st = 0;
  counter_sys_in++;
  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ( (st & KBC_OUT_BUF_FULL) != 0) return true;
  return false;

}

bool (kbc_intput_buf_full)() {
  uint8_t status_reg = 0;
  util_sys_inb(KBC_STAT_REG, &status_reg);
  counter_sys_in++;
  if (status_reg & KBC_IN_BUF_FULL) return true; 
  return false;
}

uint8_t (kbc_read_output_buffer)() {
  uint8_t scan_code;
  util_sys_inb(KBC_OUT_BUF, &scan_code);
  counter_sys_in++;
  return scan_code;
}

uint8_t (read_command_byte)() {
  int counterCycles = 0;
  uint8_t comm = 0;

  while (counterCycles < 100000) {
    if (!kbc_intput_buf_full()) {
      sys_outb(KBC_STAT_REG, KBC_READ_CMD_BYTE);
      counterCycles++;
      break;
    }
  }
  
  util_sys_inb(KBC_OUT_BUF, &comm);
  counter_sys_in++;
  return comm;
}

void (write_command_byte)(uint8_t command_byte) {
  int counterCycles = 0;
  
  while (counterCycles < 100000) {
    counterCycles++;
    if (!kbc_intput_buf_full()) {
      sys_outb(KBC_STAT_REG, KBC_WRITE_CMD_BYTE); 
      break;
    }
  }

  sys_outb(KBC_OUT_BUF, command_byte);
}



void (enable_interrupts)() {
  uint8_t command_byte = 0;
  
  command_byte = read_command_byte();
  write_command_byte(command_byte | KBC_COMMAND_ENABLE_INTERRUPTS);
  
}


/**
 * Handles keyboard interrupts (C implementation)
 * Reads the status register and the output buffer (OB).
 * If there was some error, the byte read from the OB should be discarded.
 * All communication with other code must be done via global variables, static if possible.
 */
void (kbc_ih)(void) {
  if (!kbc_output_buf_full()) return;

  uint8_t scanCode = kbc_read_output_buffer();

  if (kbc_communication_error()) return;

  if (scanCode == KBC_SCANCODE_2B) {
    flag2Bytes = true;
    return;
  }

  uint8_t arr[2];
  uint8_t size;

  if (flag2Bytes) {
    arr[1] = KBC_SCANCODE_2B;
    size = 2;
    flag2Bytes = false;
  } else {
    size = 1;
  }

  arr[0] = scanCode;
  bool make = ( (scanCode & MAKE_SCAN_CODE) == 0 );
  kbd_print_scancode(make, size, arr);
  if (scanCode == ESC_BREAK) flagESC = false;

}


int (kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBC_IRQ;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id);
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&kbc_hook_id);
}

