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



void (vg_ih)() {
  switch (game_state) {
    case PLAYING:
      vg_draw_game();
      break;
    
    default:
      break;
  }
}



void (vg_draw_game)() {
  // CLEAN THE BUFFER
  memset(buffer, 0, h_res * v_res * bytes_per_pixel);

  // DRAW THE ALIENS and only after, the crosshair
  vg_draw_aliens();

  // COPY TO THE VRAM -> in the future we might try to do flipping
  memcpy(video_mem, buffer, h_res * v_res * bytes_per_pixel);

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

  char* alien_appearing_ptr = (char*) xpm_load(xpm_alien_appearing, sprite_type, &alien_appearing_info);
  char* alien_alive_ptr = (char*) xpm_load(xpm_alien_alive, sprite_type, &alien_alive_info);
  char* alien_dead_1_ptr = (char*) xpm_load(xpm_alien_dead_1, sprite_type, &alien_dead_1_info);
  char* alien_dead_2_ptr = (char*) xpm_load(xpm_alien_dead_2, sprite_type, &alien_dead_2_info);
  char* alien_dead_3_ptr = (char*) xpm_load(xpm_alien_dead_3, sprite_type, &alien_dead_3_info);
  char* alien_dead_4_ptr = (char*) xpm_load(xpm_alien_dead_4, sprite_type, &alien_dead_4_info);
  char* alien_dead_5_ptr = (char*) xpm_load(xpm_alien_dead_5, sprite_type, &alien_dead_5_info);


  struct SPRITE alien_alive = {alien_appearing_ptr, alien_appearing_info};
  struct SPRITE alien_appearing = {alien_alive_ptr, alien_alive_info};
  struct SPRITE alien_dead_1 = {alien_dead_1_ptr, alien_dead_1_info};
  struct SPRITE alien_dead_2 = {alien_dead_2_ptr, alien_dead_2_info};
  struct SPRITE alien_dead_3 = {alien_dead_3_ptr, alien_dead_3_info};
  struct SPRITE alien_dead_4 = {alien_dead_4_ptr, alien_dead_4_info};
  struct SPRITE alien_dead_5 = {alien_dead_5_ptr, alien_dead_5_info};

  alien_sprites[0] = alien_alive;
  alien_sprites[1] = alien_appearing;
  alien_sprites[2] = alien_dead_1;
  alien_sprites[3] = alien_dead_2;
  alien_sprites[4] = alien_dead_3;
  alien_sprites[5] = alien_dead_4;
  alien_sprites[6] = alien_dead_5;

  return true;
}










// not going to be used
int (vg_draw_moving_sprite)(char* sprite, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate, xpm_image_t img_info) {
  uint16_t x = xi;
  uint16_t y = yi;
  int16_t speed_x, speed_y; // in the future maybe add other type of movements, besides horizontal and vertical
  if (xf == xi) {            // but we probably need something like the slope and it will be more generalized than vertical and horizontal
    speed_x = 0; 
    speed_y = abs(speed);
  } else {
    speed_x = abs(speed);
    speed_y = 0;
  }


  //buffer = (char*) malloc(h_res * v_res * bytes_per_pixel * sizeof(uint8_t));
  int no_interrupts = 60 / fr_rate;

  uint8_t bit_no_TIMER=0;

  timer_subscribe_int(&bit_no_TIMER);
  
  int ipc_status, r= 0, counter_interrupts = 0, neg_speed_frame_counter = 0;
  uint32_t irq_set_timer = BIT(bit_no_TIMER);
  message msg;


  while (1) { //flagEsc was here
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification		               
          if (msg.m_notify.interrupts & irq_set_timer) {
            
            if (counter_interrupts < no_interrupts) {
              counter_interrupts++;
              continue;
            }
            counter_interrupts = 0;
            
            memset(buffer, 0, h_res * v_res * bytes_per_pixel);
            //vg_draw_sprite(sprite, x, y, 1, img_info);
            memcpy(video_mem, buffer, h_res * v_res * bytes_per_pixel);
            if (x == xf && y == yf) {continue;}

            if (speed > 0) {
              x +=  speed_x;
              y +=  speed_y;
            } else if (neg_speed_frame_counter == abs(speed)) {
              if (speed_x == 0) y++;
              else x++;
              neg_speed_frame_counter = 0;
            } else {
              neg_speed_frame_counter++;
            }
           
            
            if (x + img_info.width > h_res || y + img_info.height > v_res || x < 0 || y < 0) {
              x -=  speed_x;
              y -=  speed_y;
              speed_x = 0;
              speed_y = 0;
            } 
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
  }




  timer_unsubscribe_int();
  free(buffer);
  return 0;
}







