#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBC_IRQ 1 //the keyboard uses IRQ line 1

#define KBC_STAT_CHECK (BIT(7) | BIT(6))
#define MOUSE_CHECK BIT(5)
#define KBC_OUT_BUF_FULL BIT(0)
#define KBC_IN_BUF_FULL BIT(1)
#define KBC_STAT_REG 0x64
#define KBC_OUT_BUF 0x60

#define KBC_READ_CMD_BYTE 0x20
#define KBC_WRITE_CMD_BYTE 0x60

#define KBC_COMMAND_ENABLE_INTERRUPTS (BIT(0))

#define KBC_SCANCODE_2B 0xE0
#define ESC_MAKE 0x01
#define ESC_BREAK 0x81
#define P_MAKE 0x09
#define P_BREAK 0x99
#define B_MAKE 0x30
#define B_BREAK 0xB0

#define MAKE_SCAN_CODE BIT(7)


#endif 

