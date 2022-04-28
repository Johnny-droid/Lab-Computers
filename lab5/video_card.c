#include "video_card.h"

int (vg_exit1)() {
    reg86_t reg86;
    reg86.intno = 0x10;
    reg86.ah = 0x00;
    reg86.al = 0x03;
    
    if( sys_int86(&reg86) != OK ) {
        printf("vg_exit(): sys_int86() failed \n");
        return 1;
    }
    return 0;
}



