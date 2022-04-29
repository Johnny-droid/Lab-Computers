#include "video_card.h"


bool (setGraphics)(uint16_t mode) {
    reg86_t r86;
    memset(&r86, 0, sizeof(r86));
    r86.ax = (VBE_SET_MODE_AH << 8) | VBE_SET_MODE_AL;       // VBE call, function 02 -- set VBE mode
    r86.bx = BIT(14) | mode;                                 // set bit 14: linear framebuffer
    r86.intno = BIOS_SERVICE_VIDEO_CARD;

    if( sys_int86(&r86) != OK ) {
        printf("set_vbe_mode: sys_int86() failed \n");
        return false;
    }
    return true;
}

