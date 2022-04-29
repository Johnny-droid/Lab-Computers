#ifndef __VIDEOCARD_H
#define __VIDEOCARD_H

#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lcf.h>
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"
#include "keyboard2.h"


bool (setGraphics)(uint16_t mode);
int(kbd_scan)();

#endif



