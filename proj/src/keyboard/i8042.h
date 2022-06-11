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
//#define P_MAKE 0x09
#define P_BREAK 0x99
//#define B_MAKE 0x30
#define B_BREAK 0xB0



#define A_MAKE 0x1e
#define B_MAKE 0x30
#define C_MAKE 0x2e
#define D_MAKE 0x20
#define E_MAKE 0x12
#define F_MAKE 0x21
#define G_MAKE 0x22
#define H_MAKE 0x23
#define I_MAKE 0x17
#define J_MAKE 0x24
#define K_MAKE 0x25
#define L_MAKE 0x26
#define M_MAKE 0x32
#define N_MAKE 0x31
#define O_MAKE 0x18
#define P_MAKE 0x19
#define Q_MAKE 0x10
#define R_MAKE 0x13
#define S_MAKE 0x1f
#define T_MAKE 0x14
#define U_MAKE 0x16
#define V_MAKE 0x2f
#define W_MAKE 0x11
#define X_MAKE 0x2d
#define Y_MAKE 0x15
#define Z_MAKE 0x2c

#define DASH_MAKE 0x35
#define ENTER_MAKE 0x1c
#define DEL_MAKE 0x0e

#define MAKE_SCAN_CODE BIT(7)


#endif 

