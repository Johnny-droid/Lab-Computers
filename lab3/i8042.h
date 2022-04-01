#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBC_IRQ 1 //the keyboard uses IRQ line 1

#define KBC_STAT_CHECK BIT(7) | BIT(6)
#define KBC_STAT_REG 0x64
#define KBC_OUT_BUF 0x60

#define KBC_SCANCODE_2B 0xE0
#define ESC_MAKE 0x01
#define ESC_BREAK 0x81

#endif

