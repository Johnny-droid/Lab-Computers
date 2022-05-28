#include "game.h"



int(game_loop)() {
  game_state = PLAYING; //going to change it later
  // buffer = (char*) malloc(h_res * v_res * bytes_per_pixel * sizeof(uint8_t));

  uint8_t bit_no_KBC;
  uint8_t bit_no_TIMER;

  kbc_subscribe_int(&bit_no_KBC);
  timer_subscribe_int(&bit_no_TIMER);

  int ipc_status, r = 0;
  int counter_time_out = 0;
  uint32_t irq_set_kbc = BIT(bit_no_KBC);
  uint32_t irq_set_timer = BIT(bit_no_TIMER);
  message msg;

  while (counter_time_out < 200) { //just use for test
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



