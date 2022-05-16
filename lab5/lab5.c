// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <stdio.h>

#include "macros.h"
#include "video_card.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  if (!setGraphics(mode)) return 1;
  sleep(delay);
  if (vg_exit() != OK) return 1;
  return 0;
}
 
int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  /*
  Map the graphics mode VRAM into the process' address space and initialize the video graphics mode. The LCF function vbe_get_mode_info() can be used to obtain the relevant mode parameters for these two actions.
  Modify VRAM, by calling the functions you'll develop for this lab.
  Switch back to text mode, by calling vg_exit()
  */

  /*
  get the information of the mode and then set the mode (to get the information of the mode you can use a function already implemented, and get the information from the struct)
  use the information to map to the vram
  save bit offset and how many bits for each of the RGB (maybe save the whole struct)
  */
  
  if (!prepareGraphics(mode)) return 1;
  if (!setGraphics(mode)) return 1;
  if (vg_draw_rectangle(x, y, width, height, color) != OK) return 1; 
  kbd_scan();
  if (vg_exit() != OK) return 1;
  return 0;
  
}



int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (!prepareGraphics(mode)) return 1;
  if (!setGraphics(mode)) return 1;

  if (vg_draw_pattern(mode, no_rectangles, first, step) != OK) return 1;

  kbd_scan();
  if (vg_exit() != OK) return 1;
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint16_t mode = 0x105;
  if (!prepareGraphics(mode)) return 1;
  if (!setGraphics(mode)) return 1;

  if (vg_draw_xpm(xpm, x, y) != OK) return 1;

  kbd_scan();
  if (vg_exit() != OK) return 1;
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}


/*
1. Configure the video card for the desired graphics mode
▶ Minix 3 boots in text mode, not in graphics mode
2. Write to VRAM to display on the screen what is requested
▶ Map VRAM to the process’ address space
3. Reset the video card to the text mode used by Minix
▶ You need only to call a function that we provide you
*/


// Map the physical memory of the frame buffer
/*
int r;
struct minix_mem_range mr; // physical memory range 
unsigned int vram_base; // VRAM’s physical addresss 
unsigned int vram_size; // VRAM’s size, but you can use the frame-buffer size, instead
void *video_mem; // frame-buffer VM address 
// Allow memory mapping
mr.mr_base = (phys_bytes) vram_base;
mr.mr_limit = mr.mr_base + vram_size;
if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
panic("sys_privctl (ADD_MEM) failed: %d\n", r);
// Map memory
video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
if(video_mem == MAP_FAILED)
panic("couldn’t map video memory");

*/


// Change the frame buffer to what is wanted
/*
static void *video_mem; // Address to which VRAM is mapped
static unsigned hres; // Frame horizontal resolution
static unsigned vres; // Frame vertical resolution
void vg_fill(uint32_t color) {
int i;
uint32_t *ptr; // Assuming 4 byte per pixel
ptr = video_mem;
for(i = 0; i< hres*vres; i++, ptr++) {
 // Handle a pixel at a time

*/

