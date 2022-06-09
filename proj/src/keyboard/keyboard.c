#include "keyboard.h"


int kbc_hook_id = 1;
bool flagESC;
bool flag2Bytes;


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
  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ( (st & KBC_STAT_CHECK) == 0) return false;
  return true;
}

bool (keyboard_check)() {
  uint8_t st = 0;

  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ((st & MOUSE_CHECK) == 0)
    return true;
  return false;
}

bool (kbc_output_buf_full)() {
  uint8_t st = 0;
  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ( (st & KBC_OUT_BUF_FULL) != 0) return true;
  return false;

}

bool (kbc_input_buf_full)() {
  uint8_t status_reg = 0;
  util_sys_inb(KBC_STAT_REG, &status_reg);
  if (status_reg & KBC_IN_BUF_FULL) return true; 
  return false;
}

uint8_t (kbc_read_output_buffer)() {
  uint8_t scan_code;
  util_sys_inb(KBC_OUT_BUF, &scan_code);
  return scan_code;
}

uint8_t (read_command_byte)() {
  int counterCycles = 0;
  uint8_t comm = 0;

  while (counterCycles < 100000) {
    if (!kbc_input_buf_full()) {
      sys_outb(KBC_STAT_REG, KBC_READ_CMD_BYTE);
      counterCycles++;
      break;
    }
  }
  
  util_sys_inb(KBC_OUT_BUF, &comm);
  return comm;
}

void (write_command_byte)(uint8_t command_byte) {
  int counterCycles = 0;
  
  while (counterCycles < 100000) {
    counterCycles++;
    if (!kbc_input_buf_full()) {
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
  if (!keyboard_check()) return;
  
  uint8_t scanCode = kbc_read_output_buffer();

  if (kbc_communication_error()) return;

  if (scanCode == KBC_SCANCODE_2B) {
    flag2Bytes = true;
    return;
  }


  if (flag2Bytes) {
    flag2Bytes = false;
  }


  if (scanCode == ESC_BREAK) {
    flagESC = false;
  }

  keyboard_event_handler(scanCode);
}


void (keyboard_event_handler)(uint8_t scanCode) {
  if (scanCode == ESC_BREAK) {
    game_state = EXIT;
    return;
  }

  switch (game_state) {
    case PLAYING:
      if (scanCode == P_BREAK) game_state = PAUSE;
      break;

    case PAUSE:
      if (scanCode == P_BREAK) game_state = PLAYING;
      break;

    case LB_INPUT:
      if(name[5] == 0)
        switch (scanCode)
        {
          case A_MAKE: strcat(name, "A"); break;
          case B_MAKE: strcat(name, "B"); break;
          case C_MAKE: strcat(name, "C"); break;
          case D_MAKE: strcat(name, "D"); break;
          case E_MAKE: strcat(name, "E"); break;
          case F_MAKE: strcat(name, "F"); break;
          case G_MAKE: strcat(name, "G"); break;
          case H_MAKE: strcat(name, "H"); break;
          case I_MAKE: strcat(name, "I"); break;
          case J_MAKE: strcat(name, "J"); break;
          case K_MAKE: strcat(name, "K"); break;
          case L_MAKE: strcat(name, "L"); break;
          case M_MAKE: strcat(name, "M"); break;
          case N_MAKE: strcat(name, "N"); break;
          case O_MAKE: strcat(name, "O"); break;
          case P_MAKE: strcat(name, "P"); break;
          case Q_MAKE: strcat(name, "Q"); break;
          case R_MAKE: strcat(name, "R"); break;
          case S_MAKE: strcat(name, "S"); break;
          case T_MAKE: strcat(name, "T"); break;
          case U_MAKE: strcat(name, "U"); break;
          case V_MAKE: strcat(name, "V"); break;
          case W_MAKE: strcat(name, "W"); break;
          case X_MAKE: strcat(name, "X"); break;
          case Y_MAKE: strcat(name, "Y"); break;
          case Z_MAKE: strcat(name, "Z"); break;
          
          case DASH_MAKE: strcat(name, "-"); break;
          
          case ENTER_MAKE: if(name[0]!='\0') game_save_and_display_lb(); break;
          
          default:
            break;
        }
      else
        if(scanCode == ENTER_MAKE && name[0]!='\0') 
          game_save_and_display_lb(); 
      break;
    case LEADERBOARD:
      if(scanCode == ENTER_MAKE)
        game_initialize();
        break;
    default:
      break;
  }
}


int (kbc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = kbc_hook_id = KBC_IRQ;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id);
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&kbc_hook_id);
}

