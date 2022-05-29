#include "game.h"

void(game_initialize)() {
  game_state = PLAYING; // going to change it later
  bool flip = true;

  struct ALIEN empty = {EMPTY, 0};
  struct ALIEN alive = {ALIVE, 100};

  for (int i = 0; i < GAME_HEIGHT_MATRIX; i++) {
    for (int j = 0; j < GAME_WIDTH_MATRIX; j++) {
      if (flip) {
        game_matrix[i][j] = empty;
      }
      else {
        game_matrix[i][j] = alive;
      }
      flip = !flip;
    }
  }
}

int(game_loop)() {

  uint8_t bit_no_KBC;
  uint8_t bit_no_TIMER;

  kbc_subscribe_int(&bit_no_KBC);
  timer_subscribe_int(&bit_no_TIMER);

  int ipc_status, r = 0;
  int counter_time_out = 0;
  uint32_t irq_set_kbc = BIT(bit_no_KBC);
  uint32_t irq_set_timer = BIT(bit_no_TIMER);
  message msg;

  game_initialize();
  while (counter_time_out < 400 && game_state == PLAYING) { // just use for test
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_kbc) { // subscribed interrupt
            kbc_ih();
          }
          if (msg.m_notify.interrupts & irq_set_timer) {
            counter_time_out++;
            vg_ih();
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
  }

  timer_unsubscribe_int();
  kbc_unsubscribe_int();
  return 0;
}
