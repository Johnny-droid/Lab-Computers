#include "video_card.h"


//Video card variables
static char* video_mem;		// Process (virtual) address to which VRAM is mapped
static char* buffer;
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bytes_per_pixel; 

static uint8_t red_mask_size;
static uint8_t green_mask_size;
static uint8_t blue_mask_size;

static uint8_t red_field_position;
static uint8_t green_field_position;
static uint8_t blue_field_position;


// Sprite variables
const enum xpm_image_type sprite_type = XPM_8_8_8;
static struct SPRITE alien_sprites[NUMBER_ALIEN_STATES];
static struct SPRITE killer_alien_sprites[4];
static struct SPRITE numbers_sprites[10];
static struct SPRITE letters_sprites[26];
static struct SPRITE symb_sprites[3];
static struct SPRITE lb_corners[4];
static struct SPRITE crosshair;
static struct SPRITE play_button[2]; //pressed and unpressed
static struct SPRITE exit_button[2];
static struct SPRITE game_over;
static struct SPRITE game_name;
static struct SPRITE paused;


static uint16_t game_over_x;
static uint16_t game_over_y;

static uint16_t game_name_x;
static uint16_t game_name_y;

static uint16_t paused_x;
static uint16_t paused_y;

// DRAW FUNCTIONS

void (vg_ih)() {
  // CLEAN THE BUFFER
  memset(buffer, 0, h_res * v_res * bytes_per_pixel);

  switch (game_state) {
    case PLAYING:
      vg_draw_game();
      break;
    case MENU:
      vg_draw_menu();
      break;
    case GAME_OVER:
      vg_draw_game_over();
      break;
    case PAUSE:
      vg_draw_paused();
      break;
    case LB_INPUT:
      vg_draw_input_screen();
      break;
    case LEADERBOARD:
      vg_draw_leadeboard();
      break;
    default:
      break;
  }

  // COPY TO THE VRAM -> in the future we might try to do flipping
  memcpy(video_mem, buffer, h_res * v_res * bytes_per_pixel);
}

void (vg_draw_game_over)() {
  if(game_over_counter < GAME_OVER_WAIT - 60)
    vg_draw_sprite(game_over, game_over_x, game_over_y, 1);
  vg_draw_aliens();
  vg_draw_killer_alien();
  vg_draw_crosshair(1);
}

// We are probably going to add the name of the game here as well as well as an exit button
void (vg_draw_menu)() {
  vg_draw_game_name();
  vg_draw_play_button();
  vg_draw_exit_button();
  vg_draw_time_info();
  vg_draw_crosshair(1);
}

void (vg_draw_paused)() {
  vg_draw_sprite(paused, paused_x, paused_y, 1);
}

void (vg_draw_play_button)() {
  if (mouse_x < play_button_xi || mouse_x > play_button_xf || mouse_y < play_button_yi || mouse_y > play_button_yf) {
    vg_draw_sprite(play_button[1], play_button_xi, play_button_yi, 1);
  } else {
    vg_draw_sprite(play_button[0], play_button_xi, play_button_yi, 1);
  }
}

void (vg_draw_exit_button)() {
  if (mouse_x < exit_button_xi || mouse_x > exit_button_xf || mouse_y < exit_button_yi || mouse_y > exit_button_yf) {
    vg_draw_sprite(exit_button[1], exit_button_xi, exit_button_yi, 1);
  } else {
    vg_draw_sprite(exit_button[0], exit_button_xi, exit_button_yi, 1);
  }
}

void (vg_draw_game_name)() {
  vg_draw_sprite(game_name, game_name_x, game_name_y, 1);
}


void (vg_draw_game)() {
  vg_draw_aliens();
  vg_draw_points();
  vg_draw_crosshair(1);
}

void (vg_draw_aliens)() {
  
  uint16_t xi = GAME_HORIZONTAL_MARGIN;
  uint16_t yi = GAME_VERTICAL_MARGIN;
  uint16_t x = xi;
  uint16_t y = yi;
  struct SPRITE sprite;
  enum ALIEN_STATE state;

  for (int i = 0; i < GAME_HEIGHT_MATRIX; i++, y += ALIEN_HEIGHT) {
    x = xi;
    for (int j = 0; j < GAME_WIDTH_MATRIX; j++, x += ALIEN_WIDTH) {
      state = game_matrix[i][j].state;
      if (state == EMPTY) continue;

      sprite = alien_sprites[state];
      if(game_state == PLAYING) vg_draw_sprite(sprite, x + ALIEN_HORIZONTAL_MARGIN, y + ALIEN_VERTICAL_MARGIN, 1);
    }
  } 
}

void (vg_draw_killer_alien)() {
  struct SPRITE sprite = killer_alien_sprites[killer_alien.state];
  size_t x = killer_alien.x * ALIEN_WIDTH + ALIEN_HORIZONTAL_MARGIN + GAME_HORIZONTAL_MARGIN;
  size_t y = killer_alien.y * ALIEN_HEIGHT + ALIEN_VERTICAL_MARGIN + GAME_VERTICAL_MARGIN;
  uint32_t color = 0;
  uint8_t buffer_no = 1;
  char* temp_video_mem;
  char* temp_sprite = sprite.ptr;

  if (buffer_no == 0) {
    temp_video_mem = video_mem;
  } else if (buffer_no == 1) {
    temp_video_mem = buffer;
  } else {
    return;
  }

  uint32_t row_increment = (h_res - sprite.info.width) * bytes_per_pixel;
  temp_video_mem += (h_res*y + x)*bytes_per_pixel;
  
  for (uint16_t i = y; i < v_res && i < y + sprite.info.height; i++, temp_video_mem += row_increment) {
    for (uint16_t j = x; j < h_res && j < x + sprite.info.width; j++, temp_video_mem += bytes_per_pixel) {
      color = (temp_sprite[0] | temp_sprite[1] | temp_sprite[2]);
      if (color == 0) { temp_sprite += 3; continue;}
      for (unsigned int byte = 0; byte < bytes_per_pixel; byte++) {
        temp_video_mem[byte] = *temp_sprite;
        temp_sprite++;
      }
      
    }
  }
}


void (vg_draw_crosshair)(uint8_t buffer_no) {
  char* temp_video_mem;
  char* temp_sprite = crosshair.ptr;
  uint32_t color = 0;

  if (buffer_no == 0) {
    temp_video_mem = video_mem;
  } else if (buffer_no == 1) {
    temp_video_mem = buffer;
  } else {
    return;
  }
  uint16_t x = mouse_x - crosshair_half_width;
  uint16_t y = mouse_y - crosshair_half_height;
  uint32_t row_increment = (h_res - crosshair.info.width) * bytes_per_pixel;
  temp_video_mem += (h_res*y + x)*bytes_per_pixel;
  
  for (uint16_t i = y; i < v_res && i < y + crosshair.info.height; i++, temp_video_mem += row_increment) {
    for (uint16_t j = x; j < h_res && j < x + crosshair.info.width; j++, temp_video_mem += bytes_per_pixel) {
      color = (temp_sprite[0] | temp_sprite[1] | temp_sprite[2]);
      if (color == 0) { temp_sprite += 3; continue;}
      for (unsigned int byte = 0; byte < bytes_per_pixel; byte++) {
        temp_video_mem[byte] = *temp_sprite;
        temp_sprite++;
      }
      
    }
  }

}

void (vg_draw_points)() {
  unsigned int sprite_number, temp_points = points;
  uint16_t x = POINTS_WIDTH_MARGIN;
  uint16_t y = POINTS_HEIGHT_MARGIN;
  if (temp_points == 0) {
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    return;
  }

  while (temp_points != 0) {
    sprite_number = temp_points % 10;
    temp_points /= 10;
    vg_draw_sprite(numbers_sprites[sprite_number], x, y, 1);
    x -= numbers_sprites[sprite_number].info.width + POINTS_BETWEEN_MARGIN;
  }
}


int (vg_draw_sprite)(struct SPRITE sprite, uint16_t x, uint16_t y, uint8_t buffer_no) {
  char* temp_video_mem;
  char* temp_sprite = sprite.ptr;

  if (buffer_no == 0) {
    temp_video_mem = video_mem;
  } else if (buffer_no == 1) {
    temp_video_mem = buffer;
  } else {
    return 1;
  }

  uint32_t row_increment = (h_res - sprite.info.width) * bytes_per_pixel;
  //uint32_t row_increment = h_res * bytes_per_pixel;
  temp_video_mem += (h_res*y + x)*bytes_per_pixel;
  
  for (uint16_t i = y; i < v_res && i < y + sprite.info.height; i++, temp_video_mem += row_increment) {
    for (uint16_t j = x; j < h_res && j < x + sprite.info.width; j++, temp_video_mem += bytes_per_pixel) {
      for (unsigned int byte = 0; byte < bytes_per_pixel; byte++) {
        temp_video_mem[byte] = *temp_sprite;
        temp_sprite++;
      }
      
    }
  }
    
  return 0;
}
  
void (vg_draw_str)(char * str, int x0, int y0){ 
  int x = x0;
  int y = y0;
  for(int i = 0; str[i] != '\0'; i++){
    switch (str[i])
    {
    case 'A': vg_draw_sprite(letters_sprites[0], x, y, 1); break;
    case 'B': vg_draw_sprite(letters_sprites[1], x, y, 1); break;
    case 'C': vg_draw_sprite(letters_sprites[2], x, y, 1); break;
    case 'D': vg_draw_sprite(letters_sprites[3], x, y, 1); break;
    case 'E': vg_draw_sprite(letters_sprites[4], x, y, 1); break;
    case 'F': vg_draw_sprite(letters_sprites[5], x, y, 1); break;
    case 'G': vg_draw_sprite(letters_sprites[6], x, y, 1); break;
    case 'H': vg_draw_sprite(letters_sprites[7], x, y, 1); break;
    case 'I': vg_draw_sprite(letters_sprites[8], x, y, 1); break;
    case 'J': vg_draw_sprite(letters_sprites[9], x, y, 1); break;
    case 'K': vg_draw_sprite(letters_sprites[10], x, y, 1); break;
    case 'L': vg_draw_sprite(letters_sprites[11], x, y, 1); break;
    case 'M': vg_draw_sprite(letters_sprites[12], x, y, 1); break;
    case 'N': vg_draw_sprite(letters_sprites[13], x, y, 1); break;
    case 'O': vg_draw_sprite(letters_sprites[14], x, y, 1); break;
    case 'P': vg_draw_sprite(letters_sprites[15], x, y, 1); break;
    case 'Q': vg_draw_sprite(letters_sprites[16], x, y, 1); break;
    case 'R': vg_draw_sprite(letters_sprites[17], x, y, 1); break;
    case 'S': vg_draw_sprite(letters_sprites[18], x, y, 1); break;
    case 'T': vg_draw_sprite(letters_sprites[19], x, y, 1); break;
    case 'U': vg_draw_sprite(letters_sprites[20], x, y, 1); break;
    case 'V': vg_draw_sprite(letters_sprites[21], x, y, 1); break;
    case 'W': vg_draw_sprite(letters_sprites[22], x, y, 1); break;
    case 'X': vg_draw_sprite(letters_sprites[23], x, y, 1); break;
    case 'Y': vg_draw_sprite(letters_sprites[24], x, y, 1); break;
    case 'Z': vg_draw_sprite(letters_sprites[25], x, y, 1); break;

    case '0': vg_draw_sprite(numbers_sprites[0], x, y, 1); break;
    case '1': vg_draw_sprite(numbers_sprites[1], x, y, 1); break;
    case '2': vg_draw_sprite(numbers_sprites[2], x, y, 1); break;
    case '3': vg_draw_sprite(numbers_sprites[3], x, y, 1); break;
    case '4': vg_draw_sprite(numbers_sprites[4], x, y, 1); break;
    case '5': vg_draw_sprite(numbers_sprites[5], x, y, 1); break;
    case '6': vg_draw_sprite(numbers_sprites[6], x, y, 1); break;
    case '7': vg_draw_sprite(numbers_sprites[7], x, y, 1); break;
    case '8': vg_draw_sprite(numbers_sprites[8], x, y, 1); break;
    case '9': vg_draw_sprite(numbers_sprites[9], x, y, 1); break;

    case '-': vg_draw_sprite(symb_sprites[0], x, y, 1); break;
    case '/': vg_draw_sprite(symb_sprites[1], x, y, 1); break;
    case ':': vg_draw_sprite(symb_sprites[2], x+8, y+8, 1); break;

    case ' ':
      x -= letters_sprites[0].info.width - 20;
      break;

    case '\n': 
      y+= letters_sprites[0].info.height; 
      x = x0 - letters_sprites[0].info.width;
      break;

    default:
      break;
    }
    x += letters_sprites[0].info.width;
  }
}

void (vg_draw_input_screen)(){
  vg_draw_str(input_message, 100, 100);
  if(name[0]!='\0')
    vg_draw_str(name, 8*letters_sprites[0].info.width+96, letters_sprites[0].info.height+100);
  vg_draw_crosshair(1);
}

void (vg_draw_leadeboard)(){
  vg_draw_sprite(lb_corners[0], 1, 1, 1);
  vg_draw_sprite(lb_corners[1], 702, 1, 1);
  vg_draw_sprite(lb_corners[2], 702, 502, 1);
  vg_draw_sprite(lb_corners[3], 1, 502, 1);
  vg_draw_str(getLeaderBoard(&LB, str), 230, 30);
  vg_draw_str("PRESS ENTER TO CONTINUE", 50, 100+12*letters_sprites[0].info.height);
  vg_draw_crosshair(1);
}

void (vg_draw_time_info)() {
  int aux_number;
  int  hous_vg = hours;
  int  minutes_vg = minutes;
  uint16_t x = POINTS_WIDTH_MARGIN;
  uint16_t y = POINTS_HEIGHT_MARGIN+8;
  int n = 0;
  while (minutes_vg != 0) {
      n++;
    aux_number = minutes_vg % 10;
    minutes_vg /= 10;
    vg_draw_sprite(numbers_sprites[aux_number], x, y, 1);
    x -= numbers_sprites[aux_number].info.width + POINTS_BETWEEN_MARGIN;
  }
  if(n==1){
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    x -= numbers_sprites[0].info.width + POINTS_BETWEEN_MARGIN;    
  }
  if(n==0){
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    x -= numbers_sprites[0].info.width + POINTS_BETWEEN_MARGIN;
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    x -= numbers_sprites[0].info.width + POINTS_BETWEEN_MARGIN;      
  }
  vg_draw_sprite(symb_sprites[2], x+8, y+6+8, 1);
  x -= numbers_sprites[1].info.width + POINTS_BETWEEN_MARGIN;
  n=0;
  while (hous_vg != 0) {
    n++;
    aux_number = hous_vg % 10;
    hous_vg /= 10;
    vg_draw_sprite(numbers_sprites[aux_number], x, y, 1);
    x -= numbers_sprites[aux_number].info.width + POINTS_BETWEEN_MARGIN;
  }
  if(n==1){
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    x -= numbers_sprites[0].info.width + POINTS_BETWEEN_MARGIN;    
  }
  if(n==0){
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    x -= numbers_sprites[0].info.width + POINTS_BETWEEN_MARGIN;
    vg_draw_sprite(numbers_sprites[0], x, y, 1);
    x -= numbers_sprites[0].info.width + POINTS_BETWEEN_MARGIN;      
  }

}

  /*
  for (uint16_t i = y; i < v_res && i < y + sprite.info.height; i++, temp_video_mem += row_increment) {
    //maybe we can try it with void* cast
    memcpy((void*)temp_video_mem,  temp_sprite, bytes_per_pixel * (sprite.info.width));
    temp_sprite += sprite.info.width;
  }
  
  */
  
  
































// SETUP FUNCTIONS

bool (vg_prepareGraphics)(uint16_t mode) {
  vbe_mode_info_t mode_info;
  if (vbe_get_mode_info(mode, &mode_info) != OK) return 1;
  
  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  bytes_per_pixel = ((mode_info.BitsPerPixel + 7) / 8);

  red_mask_size = mode_info.RedMaskSize;
  green_mask_size = mode_info.GreenMaskSize;
  blue_mask_size = mode_info.BlueMaskSize;   

  red_field_position = mode_info.RedFieldPosition;
  green_field_position = mode_info.GreenFieldPosition;
  blue_field_position = mode_info.BlueFieldPosition;

  unsigned int vram_base = mode_info.PhysBasePtr; // VRAM’s physical addresss
  unsigned int vram_size = h_res * v_res * bytes_per_pixel; // VRAM’s size

  buffer = (char*) malloc(h_res * v_res * bytes_per_pixel * sizeof(char));

  struct minix_mem_range mr; // physical memory range
  int r;
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return false;
  }

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED) {
    panic("couldn’t map video memory");
    return false;
  }

  return true;
} 

bool (vg_setGraphics)(uint16_t mode) {
    reg86_t r86;
    memset(&r86, 0, sizeof(r86));

    r86.ax = (VBE_MODE_AH << 8) | VBE_SET_MODE_AL;       // VBE call, function 02 -- set VBE mode
    r86.bx = BIT(14) | mode;                                 // set bit 14: linear framebuffer
    r86.intno = BIOS_SERVICE_VIDEO_CARD;

    if( sys_int86(&r86) != OK ) {
        printf("set_vbe_mode: sys_int86() failed \n");
        return false;
    }
    return true;
}

bool (vg_free)() {
  free(buffer);
  return true;
}

bool (vg_load_sprites)() {
  xpm_image_t alien_appearing_info;
  xpm_image_t alien_alive_info;
  xpm_image_t alien_dead_1_info;
  xpm_image_t alien_dead_2_info;
  xpm_image_t alien_dead_3_info;
  xpm_image_t alien_dead_4_info;
  xpm_image_t alien_dead_5_info;
  xpm_image_t alien_kill_1_info;
  xpm_image_t alien_kill_2_info;
  xpm_image_t alien_kill_3_info;
  xpm_image_t crosshair_info;
  xpm_image_t number_0_info; xpm_image_t number_1_info;
  xpm_image_t number_2_info; xpm_image_t number_3_info;
  xpm_image_t number_4_info; xpm_image_t number_5_info;
  xpm_image_t number_6_info; xpm_image_t number_7_info;
  xpm_image_t number_8_info; xpm_image_t number_9_info;

  xpm_image_t letter_A_info; xpm_image_t letter_B_info;
  xpm_image_t letter_C_info; xpm_image_t letter_D_info;
  xpm_image_t letter_E_info; xpm_image_t letter_F_info;
  xpm_image_t letter_G_info; xpm_image_t letter_H_info;
  xpm_image_t letter_I_info; xpm_image_t letter_J_info;
  xpm_image_t letter_K_info; xpm_image_t letter_L_info;
  xpm_image_t letter_M_info; xpm_image_t letter_N_info;
  xpm_image_t letter_O_info; xpm_image_t letter_P_info;
  xpm_image_t letter_Q_info; xpm_image_t letter_R_info;
  xpm_image_t letter_S_info; xpm_image_t letter_T_info;
  xpm_image_t letter_U_info; xpm_image_t letter_V_info;
  xpm_image_t letter_W_info; xpm_image_t letter_X_info;
  xpm_image_t letter_Y_info; xpm_image_t letter_Z_info;
  xpm_image_t symb_dash_info; xpm_image_t symb_slash_info;
  xpm_image_t two_points_info;

  xpm_image_t lbc_1_info; xpm_image_t lbc_2_info;
  xpm_image_t lbc_4_info; xpm_image_t lbc_3_info;
  
  xpm_image_t play_button_pressed_info; xpm_image_t play_button_unpressed_info; 
  xpm_image_t exit_button_pressed_info; xpm_image_t exit_button_unpressed_info; 
  xpm_image_t game_over_info;   xpm_image_t game_name_info;   xpm_image_t paused_info;

  char* alien_appearing_ptr = (char*) xpm_load(xpm_alien_appearing, sprite_type, &alien_appearing_info);
  char* alien_alive_ptr = (char*) xpm_load(xpm_alien_alive, sprite_type, &alien_alive_info);
  char* alien_dead_1_ptr = (char*) xpm_load(xpm_alien_dead_1, sprite_type, &alien_dead_1_info);
  char* alien_dead_2_ptr = (char*) xpm_load(xpm_alien_dead_2, sprite_type, &alien_dead_2_info);
  char* alien_dead_3_ptr = (char*) xpm_load(xpm_alien_dead_3, sprite_type, &alien_dead_3_info);
  char* alien_dead_4_ptr = (char*) xpm_load(xpm_alien_dead_4, sprite_type, &alien_dead_4_info);
  char* alien_dead_5_ptr = (char*) xpm_load(xpm_alien_dead_5, sprite_type, &alien_dead_5_info);
  char* alien_kill_1_ptr = (char*) xpm_load(xpm_alien_kill_1, sprite_type, &alien_kill_1_info);
  char* alien_kill_2_ptr = (char*) xpm_load(xpm_alien_kill_2, sprite_type, &alien_kill_2_info);
  char* alien_kill_3_ptr = (char*) xpm_load(xpm_alien_kill_3, sprite_type, &alien_kill_3_info);
  char* crosshair_ptr = (char*) xpm_load(xpm_crosshair, sprite_type, &crosshair_info);
  char* number_0_ptr = (char*) xpm_load(xpm_0, sprite_type, &number_0_info);
  char* number_1_ptr = (char*) xpm_load(xpm_1, sprite_type, &number_1_info);
  char* number_2_ptr = (char*) xpm_load(xpm_2, sprite_type, &number_2_info);
  char* number_3_ptr = (char*) xpm_load(xpm_3, sprite_type, &number_3_info);
  char* number_4_ptr = (char*) xpm_load(xpm_4, sprite_type, &number_4_info);
  char* number_5_ptr = (char*) xpm_load(xpm_5, sprite_type, &number_5_info);
  char* number_6_ptr = (char*) xpm_load(xpm_6, sprite_type, &number_6_info);
  char* number_7_ptr = (char*) xpm_load(xpm_7, sprite_type, &number_7_info);
  char* number_8_ptr = (char*) xpm_load(xpm_8, sprite_type, &number_8_info);
  char* number_9_ptr = (char*) xpm_load(xpm_9, sprite_type, &number_9_info);

  char* letter_A_ptr = (char*) xpm_load(xpm_A, sprite_type, &letter_A_info);
  char* letter_B_ptr = (char*) xpm_load(xpm_B, sprite_type, &letter_B_info);
  char* letter_C_ptr = (char*) xpm_load(xpm_C, sprite_type, &letter_C_info);
  char* letter_D_ptr = (char*) xpm_load(xpm_D, sprite_type, &letter_D_info);
  char* letter_E_ptr = (char*) xpm_load(xpm_E, sprite_type, &letter_E_info);
  char* letter_F_ptr = (char*) xpm_load(xpm_F, sprite_type, &letter_F_info);
  char* letter_G_ptr = (char*) xpm_load(xpm_G, sprite_type, &letter_G_info);
  char* letter_H_ptr = (char*) xpm_load(xpm_H, sprite_type, &letter_H_info);
  char* letter_I_ptr = (char*) xpm_load(xpm_I, sprite_type, &letter_I_info);
  char* letter_J_ptr = (char*) xpm_load(xpm_J, sprite_type, &letter_J_info);
  char* letter_K_ptr = (char*) xpm_load(xpm_K, sprite_type, &letter_K_info);
  char* letter_L_ptr = (char*) xpm_load(xpm_L, sprite_type, &letter_L_info);
  char* letter_M_ptr = (char*) xpm_load(xpm_M, sprite_type, &letter_M_info);
  char* letter_N_ptr = (char*) xpm_load(xpm_N, sprite_type, &letter_N_info);
  char* letter_O_ptr = (char*) xpm_load(xpm_O, sprite_type, &letter_O_info);
  char* letter_P_ptr = (char*) xpm_load(xpm_P, sprite_type, &letter_P_info);
  char* letter_Q_ptr = (char*) xpm_load(xpm_Q, sprite_type, &letter_Q_info);
  char* letter_R_ptr = (char*) xpm_load(xpm_R, sprite_type, &letter_R_info);
  char* letter_S_ptr = (char*) xpm_load(xpm_S, sprite_type, &letter_S_info);
  char* letter_T_ptr = (char*) xpm_load(xpm_T, sprite_type, &letter_T_info);
  char* letter_U_ptr = (char*) xpm_load(xpm_U, sprite_type, &letter_U_info);
  char* letter_V_ptr = (char*) xpm_load(xpm_V, sprite_type, &letter_V_info);
  char* letter_W_ptr = (char*) xpm_load(xpm_W, sprite_type, &letter_W_info);
  char* letter_X_ptr = (char*) xpm_load(xpm_X, sprite_type, &letter_X_info);
  char* letter_Y_ptr = (char*) xpm_load(xpm_Y, sprite_type, &letter_Y_info);
  char* letter_Z_ptr = (char*) xpm_load(xpm_Z, sprite_type, &letter_Z_info);
  char* symb_dash_ptr = (char*) xpm_load(xpm_dash, sprite_type, &symb_dash_info);
  char* symb_slash_ptr = (char*) xpm_load(xpm_slash, sprite_type, &symb_slash_info);
  char* two_points_ptr = (char*) xpm_load(xpm_two_points, sprite_type, &two_points_info);

  char * lbc_1_ptr = (char*) xpm_load(xpm_lbc1, sprite_type, &lbc_1_info);
  char * lbc_2_ptr = (char*) xpm_load(xpm_lbc2, sprite_type, &lbc_2_info);
  char * lbc_3_ptr = (char*) xpm_load(xpm_lbc3, sprite_type, &lbc_3_info);
  char * lbc_4_ptr = (char*) xpm_load(xpm_lbc4, sprite_type, &lbc_4_info);

  char* play_button_pressed_ptr = (char*) xpm_load(xpm_play_button_pressed, sprite_type, &play_button_pressed_info);
  char* play_button_unpressed_ptr = (char*) xpm_load(xpm_play_button_unpressed, sprite_type, &play_button_unpressed_info);
  char* exit_button_pressed_ptr = (char*) xpm_load(xpm_exit_button_pressed, sprite_type, &exit_button_pressed_info);
  char* exit_button_unpressed_ptr = (char*) xpm_load(xpm_exit_button_unpressed, sprite_type, &exit_button_unpressed_info);
  char* game_over_ptr = (char*) xpm_load(xpm_game_over, sprite_type, &game_over_info);
  char* game_name_ptr = (char*) xpm_load(xpm_game_name, sprite_type, &game_name_info);
  char* paused_ptr = (char*) xpm_load(xpm_paused, sprite_type, &paused_info);

  struct SPRITE alien_alive = {alien_appearing_ptr, alien_appearing_info};
  struct SPRITE alien_appearing = {alien_alive_ptr, alien_alive_info};
  struct SPRITE alien_dead_1 = {alien_dead_1_ptr, alien_dead_1_info};
  struct SPRITE alien_dead_2 = {alien_dead_2_ptr, alien_dead_2_info};
  struct SPRITE alien_dead_3 = {alien_dead_3_ptr, alien_dead_3_info};
  struct SPRITE alien_dead_4 = {alien_dead_4_ptr, alien_dead_4_info};
  struct SPRITE alien_dead_5 = {alien_dead_5_ptr, alien_dead_5_info};
  struct SPRITE alien_kill_1 = {alien_kill_1_ptr, alien_kill_1_info};
  struct SPRITE alien_kill_2 = {alien_kill_2_ptr, alien_kill_2_info};
  struct SPRITE alien_kill_3 = {alien_kill_3_ptr, alien_kill_3_info};
  struct SPRITE crosshair_sprite = {crosshair_ptr, crosshair_info}; crosshair = crosshair_sprite;
  struct SPRITE number_0 = {number_0_ptr, number_0_info};
  struct SPRITE number_1 = {number_1_ptr, number_1_info};
  struct SPRITE number_2 = {number_2_ptr, number_2_info};
  struct SPRITE number_3 = {number_3_ptr, number_3_info};
  struct SPRITE number_4 = {number_4_ptr, number_4_info};
  struct SPRITE number_5 = {number_5_ptr, number_5_info};
  struct SPRITE number_6 = {number_6_ptr, number_6_info};
  struct SPRITE number_7 = {number_7_ptr, number_7_info};
  struct SPRITE number_8 = {number_8_ptr, number_8_info};
  struct SPRITE number_9 = {number_9_ptr, number_9_info};

  struct SPRITE letter_A = {letter_A_ptr, letter_A_info};
  struct SPRITE letter_B = {letter_B_ptr, letter_B_info};
  struct SPRITE letter_C = {letter_C_ptr, letter_C_info};
  struct SPRITE letter_D = {letter_D_ptr, letter_D_info};
  struct SPRITE letter_E = {letter_E_ptr, letter_E_info};
  struct SPRITE letter_F = {letter_F_ptr, letter_F_info};
  struct SPRITE letter_G = {letter_G_ptr, letter_G_info};
  struct SPRITE letter_H = {letter_H_ptr, letter_H_info};
  struct SPRITE letter_I = {letter_I_ptr, letter_I_info};
  struct SPRITE letter_J = {letter_J_ptr, letter_J_info};
  struct SPRITE letter_K = {letter_K_ptr, letter_K_info};
  struct SPRITE letter_L = {letter_L_ptr, letter_L_info};
  struct SPRITE letter_M = {letter_M_ptr, letter_M_info};
  struct SPRITE letter_N = {letter_N_ptr, letter_N_info};
  struct SPRITE letter_O = {letter_O_ptr, letter_O_info};
  struct SPRITE letter_P = {letter_P_ptr, letter_P_info};
  struct SPRITE letter_Q = {letter_Q_ptr, letter_Q_info};
  struct SPRITE letter_R = {letter_R_ptr, letter_R_info};
  struct SPRITE letter_S = {letter_S_ptr, letter_S_info};
  struct SPRITE letter_T = {letter_T_ptr, letter_T_info};
  struct SPRITE letter_U = {letter_U_ptr, letter_U_info};
  struct SPRITE letter_V = {letter_V_ptr, letter_V_info};
  struct SPRITE letter_W = {letter_W_ptr, letter_W_info};
  struct SPRITE letter_X = {letter_X_ptr, letter_X_info};
  struct SPRITE letter_Y = {letter_Y_ptr, letter_Y_info};
  struct SPRITE letter_Z = {letter_Z_ptr, letter_Z_info};
  
  struct SPRITE symb_dash = {symb_dash_ptr, symb_dash_info};
  struct SPRITE symb_slash = {symb_slash_ptr, symb_slash_info};
  
  struct SPRITE play_button_pressed = {play_button_pressed_ptr, play_button_pressed_info};
  struct SPRITE play_button_unpressed = {play_button_unpressed_ptr, play_button_unpressed_info};
  struct SPRITE exit_button_pressed = {exit_button_pressed_ptr, exit_button_pressed_info};
  struct SPRITE exit_button_unpressed = {exit_button_unpressed_ptr, exit_button_unpressed_info};
  struct SPRITE game_over_sprite = {game_over_ptr, game_over_info}; game_over = game_over_sprite;
  struct SPRITE game_name_sprite = {game_name_ptr, game_name_info}; game_name = game_name_sprite;
  struct SPRITE paused_sprite = {paused_ptr, paused_info}; paused = paused_sprite;

  struct SPRITE lbc_1 = {lbc_1_ptr, lbc_1_info};
  struct SPRITE lbc_2 = {lbc_2_ptr, lbc_2_info};
  struct SPRITE lbc_3 = {lbc_3_ptr, lbc_3_info};
  struct SPRITE lbc_4 = {lbc_4_ptr, lbc_4_info};

  struct SPRITE two_points = {two_points_ptr, two_points_info};

  alien_sprites[0] = alien_alive;
  alien_sprites[1] = alien_appearing;
  alien_sprites[2] = alien_dead_1;
  alien_sprites[3] = alien_dead_2;
  alien_sprites[4] = alien_dead_3;
  alien_sprites[5] = alien_dead_4;
  alien_sprites[6] = alien_dead_5;

  killer_alien_sprites[0] = alien_kill_1;
  killer_alien_sprites[1] = alien_kill_2;
  killer_alien_sprites[2] = alien_kill_3;
  killer_alien_sprites[3] = alien_appearing;

  numbers_sprites[0] = number_0;
  numbers_sprites[1] = number_1;
  numbers_sprites[2] = number_2;
  numbers_sprites[3] = number_3;
  numbers_sprites[4] = number_4;
  numbers_sprites[5] = number_5;
  numbers_sprites[6] = number_6;
  numbers_sprites[7] = number_7;
  numbers_sprites[8] = number_8;
  numbers_sprites[9] = number_9;

  letters_sprites[0] = letter_A;
  letters_sprites[1] = letter_B;
  letters_sprites[2] = letter_C;
  letters_sprites[3] = letter_D;
  letters_sprites[4] = letter_E;
  letters_sprites[5] = letter_F;
  letters_sprites[6] = letter_G;
  letters_sprites[7] = letter_H;
  letters_sprites[8] = letter_I;
  letters_sprites[9] = letter_J;
  letters_sprites[10] = letter_K;
  letters_sprites[11] = letter_L;
  letters_sprites[12] = letter_M;
  letters_sprites[13] = letter_N;
  letters_sprites[14] = letter_O;
  letters_sprites[15] = letter_P;
  letters_sprites[16] = letter_Q;
  letters_sprites[17] = letter_R;
  letters_sprites[18] = letter_S;
  letters_sprites[19] = letter_T;
  letters_sprites[20] = letter_U;
  letters_sprites[21] = letter_V;
  letters_sprites[22] = letter_W;
  letters_sprites[23] = letter_X;
  letters_sprites[24] = letter_Y;
  letters_sprites[25] = letter_Z;

  symb_sprites[0] = symb_dash;
  symb_sprites[1] = symb_slash;
  symb_sprites[2] = two_points;

  lb_corners[0] = lbc_1;
  lb_corners[1] = lbc_2;
  lb_corners[2] = lbc_3;
  lb_corners[3] = lbc_4;

  play_button[0] = play_button_pressed;
  play_button[1] = play_button_unpressed;

  exit_button[0] = exit_button_pressed;
  exit_button[1] = exit_button_unpressed;
  
  play_button_xi = (GAME_WIDTH >> 1) - (play_button_pressed_info.width >> 1);
  play_button_yi = ((GAME_HEIGHT >> 1) + BUTTON_MARGIN_TOP) - (play_button_pressed_info.height >> 1);
  play_button_xf = (GAME_WIDTH >> 1) + (play_button_pressed_info.width >> 1);
  play_button_yf = ((GAME_HEIGHT >> 1) + BUTTON_MARGIN_TOP) + (play_button_pressed_info.height >> 1);

  exit_button_xi = (GAME_WIDTH >> 1) - (exit_button_pressed_info.width >> 1);
  exit_button_yi = ((GAME_HEIGHT >> 1) + (BUTTON_MARGIN_TOP << 1)) - (exit_button_pressed_info.height >> 1);
  exit_button_xf = (GAME_WIDTH >> 1) + (exit_button_pressed_info.width >> 1);
  exit_button_yf = ((GAME_HEIGHT >> 1) + (BUTTON_MARGIN_TOP << 1)) + (exit_button_pressed_info.height >> 1);

  game_over_x = (GAME_WIDTH >> 1) - (game_over.info.width >> 1);
  game_over_y = (GAME_HEIGHT >> 1) - (game_over.info.height >> 1);

  game_name_x = (GAME_WIDTH >> 1) - (game_name.info.width >> 1);
  game_name_y = GAME_MARGIN_TOP;

  paused_x = (GAME_WIDTH >> 1) - (paused.info.width >> 1);
  paused_y = (GAME_HEIGHT >> 1) - (paused.info.height >> 1);

  return true;
}





