#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>
#include "i8042.h"


/** Prints the input scancode. (Already implemented in the LCF)
 * Parameters
 * make	Whether this is a make or a break code
 * size	Size in bytes of the scancode
 * bytes	Array with size elements, with the scancode bytes
 */
//int kbd_print_scancode(bool make, uint8_t size, uint8_t * bytes )	


// Prints the no. of sys_inb() calls (Already implemented in the LCF): cnt Number of sys_inb() calls
// int kbd_print_no_sysinb	(uint32_t cnt)	


/**
 * Handles keyboard interrupts (C implementation)
 * Reads the status register and the output buffer (OB).
 * If there was some error, the byte read from the OB should be discarded.
 * All communication with other code must be done via global variables, static if possible.
 */
void (kbc_ih)(void) {
    printf("Hey");
}


int (kbc_subscribe_int)(uint8_t *bit_no) {
  int res = sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &global_hook_id);
  //*bit_no = KBC_IRQ;
  return res;
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&global_hook_id);
}

