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
static struct SPRITE numbers_sprites[10];
static struct SPRITE crosshair;
static struct SPRITE play_button[2]; //pressed and unpressed
static struct SPRITE game_over;


static uint16_t game_over_x;
static uint16_t game_over_y;

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
    default:
      break;
  }

  // COPY TO THE VRAM -> in the future we might try to do flipping
  memcpy(video_mem, buffer, h_res * v_res * bytes_per_pixel);
}

void (vg_draw_game_over)() {
  vg_draw_sprite(game_over, game_over_x, game_over_y, 1);
  vg_draw_crosshair();
}

// We are probably going to add the name of the game here as well as well as an exit button
void (vg_draw_menu)() {
  vg_draw_play_button();
  vg_draw_crosshair();
}

void (vg_draw_play_button)() {

  if (mouse_x < play_button_xi || mouse_x > play_button_xf || mouse_y < play_button_yi || mouse_y > play_button_yf) {
    vg_draw_sprite(play_button[1], play_button_xi, play_button_yi, 1);
  } else {
    vg_draw_sprite(play_button[0], play_button_xi, play_button_yi, 1);
  }

}


void (vg_draw_game)() {
  // DRAW THE ALIENS and only after, the crosshair
  vg_draw_aliens();
  vg_draw_crosshair();
  vg_draw_points();
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
      vg_draw_sprite(sprite, x + ALIEN_HORIZONTAL_MARGIN, y + ALIEN_VERTICAL_MARGIN, 1);
    }
  } 
}


void (vg_draw_crosshair)() {
  vg_draw_sprite(crosshair, mouse_x-crosshair_half_width, mouse_y-crosshair_half_height, 1);
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
  char* temp_sprite = sprite.ptr ;

  if (buffer_no == 0) {
    temp_video_mem = video_mem;
  } else if (buffer_no == 1) {
    temp_video_mem = buffer;
  } else {
    return 1;
  }

  uint32_t row_increment = (h_res - sprite.info.width) * bytes_per_pixel;
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
  xpm_image_t crosshair_info;
  xpm_image_t number_0_info; xpm_image_t number_1_info;
  xpm_image_t number_2_info; xpm_image_t number_3_info;
  xpm_image_t number_4_info; xpm_image_t number_5_info;
  xpm_image_t number_6_info; xpm_image_t number_7_info;
  xpm_image_t number_8_info; xpm_image_t number_9_info;
  xpm_image_t play_button_pressed_info; xpm_image_t play_button_unpressed_info; 
  xpm_image_t game_over_info;

  char* alien_appearing_ptr = (char*) xpm_load(xpm_alien_appearing, sprite_type, &alien_appearing_info);
  char* alien_alive_ptr = (char*) xpm_load(xpm_alien_alive, sprite_type, &alien_alive_info);
  char* alien_dead_1_ptr = (char*) xpm_load(xpm_alien_dead_1, sprite_type, &alien_dead_1_info);
  char* alien_dead_2_ptr = (char*) xpm_load(xpm_alien_dead_2, sprite_type, &alien_dead_2_info);
  char* alien_dead_3_ptr = (char*) xpm_load(xpm_alien_dead_3, sprite_type, &alien_dead_3_info);
  char* alien_dead_4_ptr = (char*) xpm_load(xpm_alien_dead_4, sprite_type, &alien_dead_4_info);
  char* alien_dead_5_ptr = (char*) xpm_load(xpm_alien_dead_5, sprite_type, &alien_dead_5_info);
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
  char* play_button_pressed_ptr = (char*) xpm_load(xpm_play_button_pressed, sprite_type, &play_button_pressed_info);
  char* play_button_unpressed_ptr = (char*) xpm_load(xpm_play_button_unpressed, sprite_type, &play_button_unpressed_info);
  char* game_over_ptr = (char*) xpm_load(xpm_game_over, sprite_type, &game_over_info);

  struct SPRITE alien_alive = {alien_appearing_ptr, alien_appearing_info};
  struct SPRITE alien_appearing = {alien_alive_ptr, alien_alive_info};
  struct SPRITE alien_dead_1 = {alien_dead_1_ptr, alien_dead_1_info};
  struct SPRITE alien_dead_2 = {alien_dead_2_ptr, alien_dead_2_info};
  struct SPRITE alien_dead_3 = {alien_dead_3_ptr, alien_dead_3_info};
  struct SPRITE alien_dead_4 = {alien_dead_4_ptr, alien_dead_4_info};
  struct SPRITE alien_dead_5 = {alien_dead_5_ptr, alien_dead_5_info};
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
  struct SPRITE play_button_pressed = {play_button_pressed_ptr, play_button_pressed_info};
  struct SPRITE play_button_unpressed = {play_button_unpressed_ptr, play_button_unpressed_info};
  struct SPRITE game_over_sprite = {game_over_ptr, game_over_info}; game_over = game_over_sprite;

  alien_sprites[0] = alien_alive;
  alien_sprites[1] = alien_appearing;
  alien_sprites[2] = alien_dead_1;
  alien_sprites[3] = alien_dead_2;
  alien_sprites[4] = alien_dead_3;
  alien_sprites[5] = alien_dead_4;
  alien_sprites[6] = alien_dead_5;

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

  play_button[0] = play_button_pressed;
  play_button[1] = play_button_unpressed;
  
  play_button_xi = (GAME_WIDTH >> 1) - (play_button_pressed_info.width >> 1);
  play_button_yi = (GAME_HEIGHT >> 1) - (play_button_pressed_info.height >> 1);
  play_button_xf = (GAME_WIDTH >> 1) + (play_button_pressed_info.width >> 1);
  play_button_yf = (GAME_HEIGHT >> 1) + (play_button_pressed_info.height >> 1);

  game_over_x = (GAME_WIDTH >> 1) - (game_over.info.width >> 1);
  game_over_y = (GAME_HEIGHT >> 1) - (game_over.info.height >> 1);

  return true;
}





