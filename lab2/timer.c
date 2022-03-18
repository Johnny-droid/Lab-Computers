#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"



int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  
  uint8_t readControl = TIMER_RB_CMD | BIT(timer+1) | TIMER_RB_COUNT_;
  if (sys_outb(TIMER_CTRL, readControl) != OK) {
      return -1;
  }
  int ret;
  switch (timer)
  {
  case 0:
    ret = util_sys_inb(TIMER_0, st);
    break;
  case 1:
    ret = util_sys_inb(TIMER_1, st);
    break;
  case 2:
    ret = util_sys_inb(TIMER_2, st);
    break;
  default:
    return 1;
  }

  return ret;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val conf;
  switch (field)
  {
  case tsf_all:
    conf.byte = st;
    break;
  case tsf_initial:
    conf.byte = st;
    conf.in_mode = 14 & st;
    break;
  case tsf_mode:
    conf.byte = st;
    conf.count_mode = 3;
    break;
  case tsf_base:
    conf.byte = st;
    conf.bcd = st & 1;
    break;
  default:
    return 1;
  }
  timer_print_config(timer, field, conf);

  return 0;
}
