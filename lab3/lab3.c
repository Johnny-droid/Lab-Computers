#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "keyboard.h"
#include "../lab2/utils.c"
#include "i8042.h"
#include "../lab2/timer.c"

extern int counter_sys_in;
extern bool flag2Bytes;
extern bool flagESC;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


/**
 * Tests reading of scancodes via KBD interrupts.
 * Displays the scancodes received from the keyboard using interrupts.
 * Exits upon release of the ESC key
 */
int(kbd_test_scan)() {
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

  //unsubscribe
  kbc_unsubscribe_int();
  return kbd_print_no_sysinb(counter_sys_in);
}


/**
 * Tests reading of scancodes via polling.
 * Displays the scancodes received from the keyboard using polling.
 * Exits upon release of the ESC key
 */
int(kbd_test_poll)() {
  counter_sys_in = 0;

  flagESC = true;
  while(flagESC) { 
    kbc_ih();
  }
  
  enable_interrupts();
  return kbd_print_no_sysinb(counter_sys_in);
}


/**
 * Tests handling of more than one interrupt.
 * Similar to kbd_test_scan() except that it should terminate also if no scancodes are received for n seconds
 */
int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_KBC=1;
  uint8_t bit_no_TIMER=0;

  kbc_subscribe_int(&bit_no_KBC);
  timer_subscribe_int(&bit_no_TIMER);

  
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no_KBC);
  uint32_t timer0_int_bit = BIT(bit_no_TIMER);
  int r = 0;
  flagESC = true;
  flag2Bytes = false;
  counter_sys_in = 0;
  int counter = 0;

   while(flagESC && counter/60 < n) { // You may want to use a different condition
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
            counter=0;
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { // Timer0 int?
            /* process Timer0 interrupt request */
            counter++;
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    } 
  }

  //unsubscribe
  timer_unsubscribe_int();
  kbc_unsubscribe_int();
  return kbd_print_no_sysinb(counter_sys_in);
}
