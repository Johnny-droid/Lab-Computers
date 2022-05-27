
#include "mouse.h"

int mouse_hook_id = 12;
uint8_t arr[3];
int nbytes = 0;
bool endCycle;
struct packet p;
/*
void (mouse_check_pattern)(mouse_event_t evt) {
  switch (pattern_state) {
    case INIT:
      if ((evt.type == LBDOWN && !evt.rbdown && !evt.mbdown) || (evt.type == MOVE && evt.lbdown && !evt.rbdown && !evt.mbdown)) {
        lineXLen = 0;
        pattern_state = DRAW_UP;
      } //else we stay on INIT
      break;
    case DRAW_UP:
      if (evt.type == MOVE) {
        lineXLen += evt.moveX;
        if (!evt.lbdown && !evt.mbdown && evt.rbdown && lineXLen >= x_len) { // although a move type, we allow the button change
          pattern_state = DRAW_DOWN;
        }
        else if (evt.moveX <= 0 || evt.moveY <= 0 || (evt.moveY / evt.moveX) < MIN_SLOPE || !evt.lbdown || evt.mbdown || (evt.rbdown && lineXLen < x_len)) {
        // note that lifting left button and pressing right is handled before
          pattern_state = INIT;
        }
      } // evt.type == MOVE
      else if (evt.type == RBDOWN) { // change the line draw
        if (!evt.lbdown && !evt.mbdown && // it was a right click, but we're extra careful
        evt.rbdown && lineXLen >= x_len) {
          lineXLen = 0;
          pattern_state = DRAW_DOWN;
        }
        else { // does not comply, right click with other buttons or length of UP too short
          pattern_state = INIT;
        }
      } // evt.type == RBDOWN
      else { // other event type goes to restart
        pattern_state = INIT;
      }
      break; // from case DRAW_UP
    case DRAW_DOWN:
      if (evt.type == MOVE) {
        lineXLen += evt.moveX;
        if (!evt.lbdown && !evt.mbdown && evt.rbdown && lineXLen >= x_len) { // although a move type, we allow the button change
          pattern_state = DETECTED;
        }
        else if (evt.moveX >= 0 || evt.moveY >= 0 || (evt.moveY / evt.moveX) < MIN_SLOPE || !evt.rbdown || evt.mbdown || (evt.lbdown && lineXLen < x_len)) {
        // note that lifting left button and pressing right is handled before
          pattern_state = INIT;
        }
      } // evt.type == MOVE
      else if (evt.type == RBDOWN) { // change the line draw
        if (!evt.lbdown && !evt.mbdown && // it was a right click, but we're extra careful
        evt.rbdown && lineXLen >= x_len) {
          pattern_state = DETECTED;
        }
        else { // does not comply, right click with other buttons or length of UP too short
          pattern_state = INIT;
        }
      } // evt.type == RBDOWN
      else { // other event type goes to restart
        pattern_state = INIT;
      }
      break; // from case DRAW_UP
    case DETECTED:
      endCycle=true;
      break;
  }
}

*/
bool(kbc_intput_buf_full)() {
  uint8_t status_reg = 0;
  util_sys_inb(KBC_STAT_REG, &status_reg);
  if (status_reg & KBC_IN_BUF_FULL)
    return true;
  return false;
}

bool(kbc_communication_error)() {
  uint8_t st = 0;
  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ((st & KBC_STAT_CHECK) == 0)
    return false;
  return true;
}

bool(mouse_check)() {
  uint8_t st = 0;

  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ((st & MOUSE_CHECK) == 0)
    return false;
  return true;
}

uint8_t(kbc_read_output_buffer)() {
  uint8_t byte;
  util_sys_inb(KBC_OUT_BUF, &byte);
  return byte;
}

bool(kbc_output_buf_full)() {
  uint8_t st = 0;
  if (util_sys_inb(KBC_STAT_REG, &st) != OK) {
    return true;
  }

  if ((st & KBC_OUT_BUF_FULL) != 0)
    return true;
  return false;
}

void(mouse_ih)() {
  if (nbytes == 0)
    memset(&arr, 0, sizeof(arr));
  if (!mouse_check())
    return;
  if (!kbc_output_buf_full())
    return;
  uint8_t byte = kbc_read_output_buffer();
  if (kbc_communication_error())
    return;
  // If the first biyte's bit 3 is not set
  if ((byte & BIT(3)) == 0 && nbytes == 0)
    return;
  arr[nbytes] = byte;
  nbytes++;
  if (nbytes == 3) {
    p.bytes[0] = arr[0];
    p.bytes[1] = arr[1];
    p.bytes[2] = arr[2];
    p.rb = (arr[0] & RB_MOUSE) >> 1;
    p.lb = (arr[0] & LB_MOUSE);
    p.mb = (arr[0] & MB_MOUSE) >> 2;
    p.x_ov = (arr[0] & X_OV_MOUSE) >> 6;
    p.y_ov = (arr[0] & Y_OV_MOUSE) >> 7;
    int16_t x = (int16_t) arr[1];
    int16_t y = (int16_t) arr[2];
    // Check if X is negative
    if (arr[0] & BIT(4))
      x = x | 0xFF00;
    // Check if Y is negative
    if (arr[0] & BIT(5))
      y = y | 0xFF00;
    p.delta_x = x;
    p.delta_y = y;
  }
}

int(read_ACK_byte)() {
  uint8_t ack_byte;
  uint8_t stat;

  while (1) {
    if (util_sys_inb(KBC_STAT_REG, &stat) != 0)
      return -1;

    if (stat & KBC_OUT_BUF_FULL)
      util_sys_inb(KBC_OUT_BUF, &ack_byte);

    // Checks acknowledgment byte
    if (ack_byte == ACK)
      return 0;
    else if (ack_byte == ERROR)
      return -1;
    else if (ack_byte == NACK)
      return 1;
    
    //tickdelay(micros_to_ticks(DELAY));
  }

}

int(mouse_disable_data_reporting)(int mode) {
  int r;
  bool try_again = true;
  while (try_again) {
    try_again = false;
    int counterCycles = 0;
    while (counterCycles < 10) {
      if (!kbc_intput_buf_full()) {
        sys_outb(KBC_STAT_REG, WR_MOUSE);
        break;
      }
      counterCycles++;
      tickdelay(micros_to_ticks(DELAY));
    }

    sys_outb(KBC_OUT_BUF, DISABLE);
    r = read_ACK_byte();
    if (r == 1)
      try_again = true;
  }

  return r;
}

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = mouse_hook_id = MOUSE_IRQ;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&mouse_hook_id);
}
