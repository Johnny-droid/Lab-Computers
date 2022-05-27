#ifndef _LCOM_PS2_H_
#define _LCOM_PS2_H_

#include <lcom/lcf.h>

#define MOUSE_IRQ 12 //the mouse uses IRQ line 12

#define KBC_STAT_CHECK (BIT(7) | BIT(6))
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
#define MAKE_SCAN_CODE BIT(7)

#define  MOUSE_CHECK BIT(5)
#define  ACK 0xFA // if everything OK
#define  NACK 0xFE // if invalid byte (may be due to a serial communication error)
#define  ERROR 0xFC // second consecutive invalid byte
#define RB_MOUSE BIT(1)
#define MB_MOUSE BIT(2)
#define LB_MOUSE BIT(0)
#define Y_OV_MOUSE BIT(7)
#define X_OV_MOUSE BIT(6)
#define WR_MOUSE     0xD4
#define RM_MODE      0xF0
#define DISABLE      0xF5



#define DELAY 20000






#endif 
