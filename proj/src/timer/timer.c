#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int timer_global_counter;
int timer_hook_id;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  printf("Timer: %d\n", timer);
  uint8_t st = 0;
  if (timer_get_conf(timer, &st) != OK) {
    return 1;
  }

  st &= 0x0F;
  printf("Timer control word 4 lsb: %X\n", st);
  st |=  TIMER_CTRL_SEL(timer) | TIMER_LSB_MSB;
  printf("Timer control word: %X\n", st);

  uint16_t timer_counter = (uint16_t) (TIMER_FREQ / freq);
  uint8_t lsb = 0, msb = 0;
  util_get_LSB(timer_counter, &lsb);
  util_get_MSB(timer_counter, &msb);

  if (sys_outb(TIMER_CTRL, st) != OK) {
    return 1;
  }
  
  int res1, res2;
  switch (timer) {
    case 0:
      res1 = sys_outb(TIMER_0, lsb);
      res2 = sys_outb(TIMER_0, msb);
      break;
    case 1:
      res1 = sys_outb(TIMER_1, lsb);
      res2 = sys_outb(TIMER_1, msb);
      break;
    case 2:
      res1 = sys_outb(TIMER_2, lsb);
      res2 = sys_outb(TIMER_2, msb);
      break;
    default:
      return 1;
  }

  return res1 & res2;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer_hook_id = TIMER0_IRQ;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)() {
  timer_global_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t command = TIMER_RB_CMD |  TIMER_RB_COUNT_  | BIT(timer + 1);
  if (sys_outb(TIMER_CTRL, (uint32_t) command) != OK ) {
    return 1;
  }

  switch (timer) {
    case 0:
      return util_sys_inb(TIMER_0, st);
    case 1:
      return util_sys_inb(TIMER_1, st);
    case 2:
      return util_sys_inb(TIMER_2, st);
    default:
      return 1;
  }
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val val;

  val.byte = st;

  switch (field) {
    case tsf_all:
      val.byte = st;
      break; 

    case tsf_initial:   // Bit 5 and 4 of the control word show the counter initialization
      if (  ((st & BIT(5)) != 0)   &&  ((st & BIT(4)) != 0)  ) { 
        val.in_mode = MSB_after_LSB;
      } else if ( (st & BIT(5)) != 0 ) {
        val.in_mode = MSB_only;
      } else if ( (st & BIT(4)) != 0 ) {
        val.in_mode = LSB_only;
      } else {
        val.in_mode = INVAL_val;
      }
      break;

    case tsf_mode:
      st = st >> 1; 
      st &= 0x07;
      if (st == 0) {val.count_mode = 0;}
      else if (st == 1) {val.count_mode = 1;}
      else if (st == 2) {val.count_mode = 2;}  // check if it's a Rate generator
      else if (st == 3) {val.count_mode = 3;}  // check if it's Square wave generator 
      else if (st == 4) {val.count_mode = 4;} 
      else {val.count_mode = 5;}
      printf("Counting Mode: %d\n", val.count_mode);
      break;

    case tsf_base:     //true if counting in BCD
      if ((st & TIMER_BCD) != 0) val.bcd = true;
      else val.bcd = false;
      break;

    default:
      return 1; 
  }

  if ( timer_print_config(timer, field, val) != OK) {
    return 1;
  }

  return 0;
}



void (delay_seconds)(uint8_t seconds) {
  uint8_t bit_no;
  timer_subscribe_int(&bit_no);

  int r;
  uint8_t counter_seconds = 0;
  int ipc_status;
  message msg;
  timer_global_counter = 0;
  uint32_t irq_set = BIT(bit_no);

  while( counter_seconds < seconds ) { /* You may want to use a different condition */
    /* Get a request message. */
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { 
            timer_int_handler();
            if (timer_global_counter % 60 == 0) {
              timer_print_elapsed_time();
              counter_seconds++;
            }
          }
          break;
        default:
          break; 
      }
    }
  }
  timer_unsubscribe_int();
}


