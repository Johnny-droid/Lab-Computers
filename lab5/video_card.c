#include "video_card.h"

//Keyboard variables
extern int counter_sys_in;
extern bool flag2Bytes;
extern bool flagESC;


//Video card variables
static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bytes_per_pixel; 

static uint8_t red_mask_size;
static uint8_t green_mask_size;
static uint8_t blue_mask_size;


bool (prepareGraphics)(uint16_t mode) {
  // set mode and save the information of the mode
  vbe_mode_info_t mode_info;
  if (vbe_get_mode_info(mode, &mode_info) != OK) return 1;
  
  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;
  bytes_per_pixel = (mode_info.BitsPerPixel / 8);

  red_mask_size = mode_info.RedMaskSize;
  green_mask_size = mode_info.GreenMaskSize;
  blue_mask_size = mode_info.BlueMaskSize;   

  unsigned int vram_base = mode_info.PhysBasePtr; // VRAM’s physical addresss
  unsigned int vram_size = h_res * v_res * bytes_per_pixel; // VRAM’s size, but you can use the frame-buffer size, instead
  //uint32_t *video_mem; frame-buffer VM address

  // Allow memory mapping
  /* Use VBE function 0x01 to initialize vram_base and vram_size */
  struct minix_mem_range mr; // physical memory range
  int r;
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return false;
  }
  
  // Map memory
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  
  if(video_mem == MAP_FAILED) {
    panic("couldn’t map video memory");
    return false;
  }

  return true;
}

bool (setGraphics)(uint16_t mode) {
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


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  char* temp_video_mem = video_mem;

  if (x > h_res || y > v_res) return 1;

  temp_video_mem += ((y * h_res) + x) * bytes_per_pixel;

  for (unsigned int i = x; i < h_res && i < x + len; i++) {
    *temp_video_mem = color;
    temp_video_mem += bytes_per_pixel;
  }

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  uint8_t ret = 0;
  for (unsigned int i = y; i < v_res && i < y + height; i++) {
    ret |= vg_draw_hline(x, i, width, color);
  }

  return ret;
}



uint32_t (calculateColorPatternIndexed)(uint8_t no_rectangles, unsigned int x, unsigned int y, uint32_t first, uint8_t step) {
  return (first + (y  * no_rectangles + x) * (uint32_t) step) % (1 << (bytes_per_pixel * 8));
}

uint32_t (getBlueBits)(uint32_t color) {
  uint32_t blue_bits = 0;
  for (unsigned int i = 0; i < blue_mask_size; i++) {blue_bits |= BIT(i);}
  return color & blue_bits;
}

uint32_t (getGreenBits)(uint32_t color) {
  uint32_t green_bits = 0;
  for (unsigned int i=0; i < green_mask_size; i++) {green_bits |= BIT(i);}
  return color & (green_bits << blue_mask_size);
}

uint32_t (getRedBits)(uint32_t color) {
  uint32_t red_bits = 0;
  for (unsigned int i=0; i < red_mask_size; i++) {red_bits |= BIT(i);}
  return color & (red_bits << (blue_mask_size + green_mask_size));
}

uint32_t (calculateColorPatternDirect)(uint8_t no_rectangles, unsigned int x, unsigned int y, uint32_t first, uint8_t step) {
  uint32_t blue_bits = ((getBlueBits(first)) + (x + y) * step) % (1 << blue_mask_size);
  uint32_t green_bits = ((getGreenBits(first) + y * step) % (1 << green_mask_size)) << (blue_mask_size);
  uint32_t red_bits = ((getRedBits(first) + x * step) % (1 << red_mask_size)) << (blue_mask_size + green_mask_size);
  return red_bits | green_bits | blue_bits;
}

int (vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  unsigned int x_pixels_per_rectangle = h_res / no_rectangles;
  unsigned int y_pixels_per_rectangle = v_res / no_rectangles;
  uint32_t color;

  //color = calculateColorPatternIndexed(no_rectangles, 1, 1, first, step);
  //vg_draw_rectangle(1 * x_pixels_per_rectangle, 1 * y_pixels_per_rectangle, x_pixels_per_rectangle, y_pixels_per_rectangle, color);

  
  if (mode == 0x105) {
    for (unsigned int y = 0; y < no_rectangles; y++) {
      for (unsigned int x = 0; x < no_rectangles; x++) {
        if ((x+1)*x_pixels_per_rectangle >= h_res  ||  (y+1)*y_pixels_per_rectangle >= v_res) continue;
        color = calculateColorPatternIndexed(no_rectangles, x, y, first, step);
        vg_draw_rectangle(x * x_pixels_per_rectangle, y * y_pixels_per_rectangle, x_pixels_per_rectangle, y_pixels_per_rectangle, color);
      }
    }  
  
  } else {
    for (unsigned int y = 0; y < no_rectangles; y++) {
      for (unsigned int x = 0; x < no_rectangles; x++) {
        if ((x+1)*x_pixels_per_rectangle >= h_res  ||  (y+1)*y_pixels_per_rectangle >= v_res) continue;
        color = calculateColorPatternDirect(no_rectangles, x, y, first, step);
        vg_draw_rectangle(x * x_pixels_per_rectangle, y * y_pixels_per_rectangle, x_pixels_per_rectangle, y_pixels_per_rectangle, color);
      }
    }  
  }
  

  return 0;
}


int(kbd_scan)() {
  uint8_t bit_no;

  kbc_subscribe_int(&bit_no);

  int ipc_status, r= 0;
  uint32_t irq_set = BIT(bit_no);
  message msg;
  flagESC = true;
  flag2Bytes = false;
  counter_sys_in = 0;

   while(flagESC) { // You may want to use a different condition
    // Get a request message.

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification		               
          if (msg.m_notify.interrupts & irq_set) { //subscribed interrupt                  
           // process it
            kbc_ih();
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    } 
  }

  kbc_unsubscribe_int();
  return 0;
}
