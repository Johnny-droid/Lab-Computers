#include "game.h"



static uint32_t frame_counter = 0;
static uint32_t frame_number = 60 / GAME_FRAME_RATE;

static uint32_t random_alien_counter = 0;
static uint32_t random_alien_number = ALIEN_INIT_SPAWN_TIME;
static uint32_t random_alien_spawn_rate_increase = ALIEN_SPAWN_RATE_INCREASE;


void(game_initialize)() {
  srand(time(NULL));    //used to generate random numbers for alien spawn
  game_state = MENU;    
  points = 0;
  mouse_x = MOUSE_INIT_X;
  mouse_y = MOUSE_INIT_Y;
  crosshair_half_width = (CROSSHAIR_WIDTH >> 1);  //dividing by 2
  crosshair_half_height = (CROSSHAIR_HEIGHT >> 1);  
  crosshair_width_border = (GAME_WIDTH - (CROSSHAIR_WIDTH >> 1));
  crosshair_height_border = (GAME_HEIGHT - (CROSSHAIR_HEIGHT >> 1));

  struct ALIEN empty = {EMPTY, 0};

  for (int i = 0; i < GAME_HEIGHT_MATRIX; i++) {
    for (int j = 0; j < GAME_WIDTH_MATRIX; j++) {
      game_matrix[i][j] = empty;
    }
  }
}



int(game_loop)() {

  uint8_t bit_no_KBC;
  uint8_t bit_no_TIMER;
  uint8_t bit_no_MOUSE;

  kbc_subscribe_int(&bit_no_KBC);
  timer_subscribe_int(&bit_no_TIMER);
  mouse_subscribe_int(&bit_no_MOUSE);
  mouse_enable_data_reporting();

  int ipc_status, r = 0;
  int counter_time_out = 0;
  uint32_t irq_set_kbc = BIT(bit_no_KBC);
  uint32_t irq_set_timer = BIT(bit_no_TIMER);
  uint32_t irq_set_mouse = BIT(bit_no_MOUSE);
  message msg;

  game_initialize();
  while (counter_time_out < 5000 && game_state != EXIT) { // time_out just used for test
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_mouse) {
            mouse_ih();
          }
          if (msg.m_notify.interrupts & irq_set_kbc) { // subscribed interrupt
            kbc_ih();
          }
          if (msg.m_notify.interrupts & irq_set_timer) {
            counter_time_out++;
            if (frame_counter < frame_number) {
              frame_counter++;
              break;
            }
            frame_counter = 0;
            vg_ih();
            game_ih();
          }
          
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
  }

  timer_unsubscribe_int();
  kbc_unsubscribe_int();
 
  printf("Before disable data reporting\n");
  mouse_disable_data_reporting(1);
  printf("After disable data reporting\n"); 
  mouse_unsubscribe_int();
  return 0;
}



void(game_ih)() {

  switch (game_state) {
    case PLAYING:
      game_step();
      break;

    default:
      break;
  }
}



void(game_step)() {
  game_update_alien_times();  
  game_generate_new_alien();
}


void(game_update_alien_times)() {
  // Decrease time for every alien
  struct ALIEN *alien;

  for (int i = 0; i < GAME_HEIGHT_MATRIX; i++) {
    for (int j = 0; j < GAME_WIDTH_MATRIX; j++) {
      alien = &game_matrix[i][j];
      
      // EMPTY
      if (alien->state == EMPTY) continue;

      // ALIENS KILL PLAYER
      if (alien->state == ALIVE && alien->time == 0) {
        game_state = GAME_OVER;
        return;
      }

      // DECREMENT TIME LEFT
      if (alien->time > 0) alien->time--;

      // CHANGE TO THE NEXT STATE
      else {
        alien->state++;
        alien->time = alien_times[alien->state];
      }
    }
  }
}


void(game_generate_new_alien)() {
  if (random_alien_counter < random_alien_number) {
    random_alien_counter++;
    return;
  }
  random_alien_counter = 0;
  
  if (random_alien_spawn_rate_increase > random_alien_number) {
    random_alien_number = ALIEN_MAX_SPAWN_RATE;
    random_alien_spawn_rate_increase = 0;
  }
  random_alien_number -= random_alien_spawn_rate_increase;
  

  // Tries only some times
  uint32_t pos_x, pos_y;
  uint32_t random_attemps = ALIEN_RANDOM_SPAWN_ATTEMPTS;
  struct ALIEN *alien;
  
  while (random_attemps > 0) {
    random_attemps--;
    pos_x = rand() % GAME_WIDTH_MATRIX;
    pos_y = rand() % GAME_HEIGHT_MATRIX;
    if (game_matrix[pos_y][pos_x].state == EMPTY) {
      alien = &game_matrix[pos_y][pos_x];
      alien->state = APPEARING;
      alien->time = alien_times[alien->state];
      return;
    }
  }
 

}



