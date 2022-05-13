#include "video_card.h"

extern int counter_sys_in;
extern bool flag2Bytes;
extern bool flagESC;

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

  //unsubscribe
  kbc_unsubscribe_int();
  return 0;
}
