#include "game.h"



static uint32_t frame_counter = 0;
static uint32_t frame_number = 60 / GAME_FRAME_RATE;

static uint32_t random_alien_counter;
static uint32_t random_alien_number;
static uint32_t random_alien_spawn_rate_increase;


void(game_initialize)() {
  srand(time(NULL));    //used to generate random numbers for alien spawn
  game_state = MENU;    
  points = 0;
  game_over_counter = 0;
  mouse_x = MOUSE_INIT_X;
  mouse_y = MOUSE_INIT_Y;
  crosshair_half_width = (CROSSHAIR_WIDTH >> 1);  //dividing by 2
  crosshair_half_height = (CROSSHAIR_HEIGHT >> 1);  
  crosshair_width_border = (GAME_WIDTH - (CROSSHAIR_WIDTH >> 1));
  crosshair_height_border = (GAME_HEIGHT - (CROSSHAIR_HEIGHT >> 1));

  random_alien_counter = 0;
  random_alien_number = ALIEN_INIT_SPAWN_TIME;
  random_alien_spawn_rate_increase = ALIEN_SPAWN_RATE_INCREASE;

  game_reset();
}



int(game_loop)() {

  uint8_t bit_no_KBC;
  uint8_t bit_no_TIMER;
  uint8_t bit_no_MOUSE;
  _mouse_enable_data_reporting_();
  kbc_subscribe_int(&bit_no_KBC);
  timer_subscribe_int(&bit_no_TIMER);
  mouse_subscribe_int(&bit_no_MOUSE);
  rtc_ih();

  int ipc_status, r = 0;
  uint32_t irq_set_kbc = BIT(bit_no_KBC);
  uint32_t irq_set_timer = BIT(bit_no_TIMER);
  uint32_t irq_set_mouse = BIT(bit_no_MOUSE);
  message msg;

  game_initialize();
  readSaveFile(&LB);

  while (game_state != EXIT) {
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
            frame_counter++;
            if (frame_counter % frame_number == 0) {
              if (game_state == MENU) rtc_ih();
              vg_ih();
              game_ih();
            }
            
          }
          
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
  }

  writeSaveFile(&LB);

  timer_unsubscribe_int();
  kbc_unsubscribe_int();
  mouse_unsubscribe_int();
  mouse_disable_data_reporting(1);
  return 0;
}


void(game_ih)() {

  switch (game_state) {
    case PLAYING:
      game_step();
      break;
    case GAME_OVER:
      game_update_alien_times();
      game_over_counter--;
      killer_alien.state = (game_over_counter/5) % 4;
      if (game_over_counter == 0) game_leaderboard();
      break;
    default:
      break;
  }
}


void (game_leaderboard)(){
  memset(name, 0, sizeof name);
  memset(input_message, 0, sizeof input_message);
  strcpy(input_message, "SCORE:   ");
  char sc[4];
  sprintf(sc, "%d", points);
  strcat(input_message, sc);
  if(compareScore(points, LB)){
    strcat(input_message, "\nNAME:   \n\nTYPE AND PRESS ENTER");
    game_state = LB_INPUT;
    return;
  }
  game_state = LEADERBOARD;
}


void (game_save_and_display_lb)(){
  addScore(points, name, &LB);
  game_state = LEADERBOARD;
}


void(game_step)() {
  game_update_alien_times();  
  game_generate_new_alien();
}


void (game_reset)() {
  struct ALIEN empty = {EMPTY, 0};

  for (int i = 0; i < GAME_HEIGHT_MATRIX; i++) {
    for (int j = 0; j < GAME_WIDTH_MATRIX; j++) {
      game_matrix[i][j] = empty;
    }
  }
}



void(game_update_alien_times)() {
  // Decrease time for every alien
  struct ALIEN *alien;

  for (size_t i = 0; i < GAME_HEIGHT_MATRIX; i++) {
    for (size_t j = 0; j < GAME_WIDTH_MATRIX; j++) {
      alien = &game_matrix[i][j];
      
      // EMPTY
      if (alien->state == EMPTY) continue;

      if(game_state == GAME_OVER){
        if(killer_alien.x == j && killer_alien.y == i) continue;
        if (alien->time > 0) alien->time--;
        else{
          alien->state++;
          alien->time = 2;
        }
        continue;
      }

      // ALIENS KILL PLAYER
      if (alien->state == ALIVE && alien->time == 0) {
        game_over_counter = GAME_OVER_WAIT;
        game_state = GAME_OVER;
        killer_alien.state = 0;
        killer_alien.x = j;
        killer_alien.y = i;
        kill_other_aliens();
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



void (kill_other_aliens)(){
  struct ALIEN *alien;
  for (size_t i = 0; i < GAME_HEIGHT_MATRIX; i++) {
    for (size_t j = 0; j < GAME_WIDTH_MATRIX; j++) {
      alien = &game_matrix[i][j];
      
      if (alien->state == EMPTY || (killer_alien.x == j && killer_alien.y == i)) continue;
      
      alien->state = DEAD_1;
      alien->time = 2;
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



