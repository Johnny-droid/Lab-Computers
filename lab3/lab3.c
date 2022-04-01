#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "keyboard.h"
#include "../lab2/utils.c"
#include "i8042.h"


extern int global_hook_id;
extern bool flag_cycle_ESC;

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
  /* To be completed by the students */
  //Subscribe to the interrupts
  uint8_t bit_no=1;

  global_hook_id = KBC_IRQ;

  kbc_subscribe_int(&bit_no);

  int counter = 0;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  int r;
  flag_cycle_ESC = true;


   while(flag_cycle_ESC) { // You may want to use a different condition
    // Get a request message.
    
    printf("Entrou no loop\n");

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

     printf("Apos if\n");
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification		               
          if (msg.m_notify.interrupts & irq_set) { //subscribed interrupt                  
           // process it
            kbc_ih();
            printf("Apos kbc_ih\n");
            counter++;
          }
          break;

        default:
          break; // no other notifications expected: do nothing
      }
    }

    
  }


  //unsubscribe
  kbc_unsubscribe_int();


  return kbd_print_no_sysinb(counter);
}


/**
 * Tests reading of scancodes via polling.
 * Displays the scancodes received from the keyboard using polling.
 * Exits upon release of the ESC key
 */
int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}


/**
 * Tests handling of more than one interrupt.
 * Similar to kbd_test_scan() except that it should terminate also if no scancodes are received for n seconds
 */
int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
