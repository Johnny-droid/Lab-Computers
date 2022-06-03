// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "../lab2/utils.c"
#include "ps2.h"
#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "../lab2/timer.c"

extern int nbytes;
extern bool endCycle;
extern struct packet p; 
extern int timer_global_counter; 


// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no_MOUSE=12;
  mouse_subscribe_int(&bit_no_MOUSE);
  _mouse_enable_data_reporting_();
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no_MOUSE);
  int r = 0;
 
  int counter = 0;
   while( counter < (int) cnt) { // You may want to use a different condition
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
            mouse_ih();
            if(nbytes==3){
               counter++;
               nbytes=0;
               mouse_print_packet(&p); 
            }
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    } 
  }
  
  //unsubscribe
  mouse_disable_data_reporting(1);
  mouse_unsubscribe_int();
  
  
  
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {  
  uint8_t bit_no_MOUSE=12;
  uint8_t bit_no_TIMER=0;

  mouse_subscribe_int(&bit_no_MOUSE);
  mouse_enable_data_reporting();
  timer_subscribe_int(&bit_no_TIMER);

  
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no_MOUSE);
  uint32_t timer0_int_bit = BIT(bit_no_TIMER);
  int r = 0;

  timer_global_counter  = 0;

   while(timer_global_counter/60 < idle_time) { // You may want to use a different condition
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
            timer_global_counter=0;
            mouse_ih();
            if(nbytes==3){
               nbytes=0;
               mouse_print_packet(&p); 
            }
          }
          if (msg.m_notify.interrupts & timer0_int_bit) { // Timer0 int?
            /* process Timer0 interrupt request */
            timer_int_handler();
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    } 
  }

  //unsubscribe
  timer_unsubscribe_int();
  mouse_disable_data_reporting(1);
  mouse_unsubscribe_int();
  return 1;
}
/*
int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t bit_no_MOUSE=12;
  mouse_subscribe_int(&bit_no_MOUSE);
  mouse_enable_data_reporting();
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no_MOUSE);
  int r = 0;
  static state_t pattern_state = INIT; // initial state; keep state
  static int lineXLen = 0; // line movement in X
  endCycle=false;
   while( !endCycle) { // You may want to use a different condition
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
            mouse_ih();
            if(nbytes==3){
              nbytes=0;
              
            }
          }
          break;
        default:
          break; // no other notifications expected: do nothing
      }
    } 
  }
  //unsubscribe
  mouse_disable_data_reporting(1);
  mouse_unsubscribe_int();
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    //This year you don´t need to implement this. 
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
*/
