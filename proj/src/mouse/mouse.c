
#include "mouse.h"

static int mouse_hook_id;
uint8_t arr[3];
int nbytes = 0;
bool endCycle;
struct packet p;



void(mouse_ih)() {
  if (nbytes == 0)
    memset(&arr, 0, sizeof(arr));
  if (keyboard_check())
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
    struct MOUSE_EVENT mouse_event;
    //p.bytes[0] = arr[0];
    //p.bytes[1] = arr[1];
    //p.bytes[2] = arr[2];
    //p.rb = (arr[0] & RB_MOUSE) >> 1;
    //p.lb = (arr[0] & LB_MOUSE);
    mouse_event.lbdown = (arr[0] & LB_MOUSE);
    //p.mb = (arr[0] & MB_MOUSE) >> 2;
    //p.x_ov = (arr[0] & X_OV_MOUSE) >> 6;
    //p.y_ov = (arr[0] & Y_OV_MOUSE) >> 7;
    mouse_event.overflow = ((arr[0] & X_OV_MOUSE) >> 6) || ((arr[0] & Y_OV_MOUSE) >> 7);
    int16_t x = (int16_t) arr[1];
    int16_t y = (int16_t) arr[2];
    // Check if X is negative
    if (arr[0] & X_SIGN)
      x = x | 0xFF00;
    // Check if Y is negative
    if (arr[0] & Y_SIGN)
      y = y | 0xFF00;
    //p.delta_x = x;
    //p.delta_y = y;
    mouse_event.moveX = x;
    mouse_event.moveY = y;
    mouse_event_handler(mouse_event);
    nbytes = 0;
  }
}


void (mouse_event_handler)(struct MOUSE_EVENT mouse_event) {
  if (mouse_event.overflow) return;

  mouse_x += mouse_event.moveX;
  mouse_y -= mouse_event.moveY;
  //printf("Mouse x: %d\t Mouse y: %d\n", mouse_x, mouse_y);

  if (mouse_x < crosshair_half_width) mouse_x = crosshair_half_width;
  if (mouse_x >= crosshair_width_border) mouse_x = crosshair_width_border-1;
  if (mouse_y < crosshair_half_height) mouse_y = crosshair_half_height;
  if (mouse_y >= crosshair_height_border) mouse_y = crosshair_height_border-1;

  switch (game_state) {
    case PLAYING:
      if (mouse_event.lbdown) mouse_check_kill();
      break;
    
    default:
      break;
  }
}


void (mouse_check_kill)() {
  int x = (mouse_x - GAME_HORIZONTAL_MARGIN) / ALIEN_WIDTH;
  int y = (mouse_y - GAME_VERTICAL_MARGIN)  / ALIEN_HEIGHT;

  if (x >= GAME_WIDTH_MATRIX || x < 0) return;
  if (y >= GAME_HEIGHT_MATRIX || y < 0) return;

  struct ALIEN* alien = &game_matrix[y][x];

  // Ignores if there is no alien in the place of the mouse
  if (alien->state == EMPTY) {
    return;

  // Kills the alien
  } else if (alien->state == ALIVE || alien->state == APPEARING) {
    alien->state = DEAD_1;
    alien->time = alien_times[DEAD_1];
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
  }

}

int(mouse_disable_data_reporting)(int mode) {
  int r;
  bool try_again = true;
  while (try_again) {
    try_again = false;
    int counterCycles = 0;
    while (counterCycles < 10) {
      if (!kbc_input_buf_full()) {
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
