#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int global_hook_id = 0;
int global_counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  // So first we need to get the 4 least significant bits, because we don't want
  // to change the configuration of the time, only the frequency. So, we need to get
  // the configuration that is already implemented

  uint8_t st = 0;
  if (timer_get_conf(timer, &st) != OK) {
    return 1;
  }

  uint8_t lsb = 0, msb = 0; 
  uint16_t valueCounter = (uint16_t) (TIMER_FREQ / freq); 
  util_get_LSB(valueCounter, &lsb);
  util_get_MSB(valueCounter, &msb);

  uint8_t controlWord = st & 0x0f; //preserve the 4 ls bits
  // Now we need to define the timer in the control word
  //Pode ser feito de maneira muito mais eficiente com shift penso eu, mas por agora não é a minha preocupação
  if (timer == 0) {controlWord |= TIMER_SEL0;}
  else if (timer == 1) {controlWord |= TIMER_SEL1;}
  else if (timer == 2) {controlWord |= TIMER_SEL2;}
  else {return 1;}

  //Define the Initialization Mode of the control word
  controlWord |= TIMER_LSB_MSB;


  if (sys_outb(TIMER_CTRL, controlWord) != OK) {
    return 1;
  }

  int ret1 = 1, ret2 = 1;

  int port = TIMER_0 + timer;

  ret1 = sys_outb(port, lsb);
  ret2 = sys_outb(port, msb);

  return ret1 | ret2;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  int res = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &global_hook_id);
  *bit_no = TIMER0_IRQ;
  return res;
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&global_hook_id);
}

void (timer_int_handler)() {
  global_counter++;  
  if(global_counter % 60 == 0) timer_print_elapsed_time();
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  
  uint8_t readControl = TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(timer+1);

  if (sys_outb(TIMER_CTRL, readControl) != OK) {
      return 1;
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

    if ((st & TIMER_LSB) != 0  &&  (st & TIMER_MSB) != 0) {
      conf.in_mode = MSB_after_LSB;
    } else if ((st & TIMER_MSB) != 0) {
      conf.in_mode = MSB_only;
    } else if ((st & TIMER_LSB) != 0) {
      conf.in_mode = LSB_only;
    } else if ((st & TIMER_LSB_MSB) == 0) {
      conf.in_mode = INVAL_val;
    }

    break;
  case tsf_mode:
    conf.byte = st;

    if (((st & BIT(3)) == 0) && ((st & BIT(2)) == 0) && ((st & BIT(1)) == 0)) {conf.count_mode = 0;}
    else if (((st & BIT(3)) == 0) && ((st & BIT(2)) == 0) && ((st & BIT(1)) == 1)) {conf.count_mode = 1;}
    else if (((st & BIT(2)) == 1) && ((st & BIT(1)) == 0)) {conf.count_mode = 2;}  // check if it's a Rate generator
    else if (((st & BIT(2)) == 1)  &&  ((st & BIT(1)) == 1)) {conf.count_mode = 3;}  // check if it's Square wave generator 
    else if (((st & BIT(3)) == 1) && ((st & BIT(2)) == 0) && ((st & BIT(1)) == 0)) {conf.count_mode = 4;} 
    else {conf.count_mode = 5;}
    break;

  case tsf_base:
    conf.byte = st;

    if ((st & TIMER_BCD) != 0) {
      conf.bcd = true;
    } else {
      conf.bcd = false;
    }
    break;

  default:
    return 1;
  }
  timer_print_config(timer, field, conf);

  return 0;
}
