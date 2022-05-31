#include <lcom/lcf.h>
#include <lcom/proj.h>

#include "video_card/video_card.h"
#include "game.h"

#include <stdint.h>
#include <stdio.h>

 
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (proj_main_loop)(int argc, char* agrv[]) {
  if (!vg_prepareGraphics(GAME_GRAPHICS_MODE)) return EXIT_FAILURE;
  if (!vg_load_sprites()) return EXIT_FAILURE;
  if (!vg_setGraphics(GAME_GRAPHICS_MODE)) return EXIT_FAILURE;  

  game_loop();

  if (vg_exit() != OK) return EXIT_FAILURE;
  if (!vg_free()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
