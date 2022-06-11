#ifndef __RTC_H
#define __RTC_H

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include <stdlib.h>
#include <stdint.h>
#include "../game.h"
#include "keyboard/keyboard.h"
#include "rtc_macros.h"

static  uint8_t g_rtc_seconds;
static  uint8_t g_rtc_minutes;
static  uint8_t g_rtc_hours;
static  uint8_t g_rtc_day;
static  uint8_t g_rtc_month;
static  uint8_t g_rtc_year;


int (get_rtc_seconds)();
uint8_t (to_BCD)(uint8_t value);
int (get_rtc_minutes)();
int (get_rtc_hours)();
int (get_rtc_day)();
int (get_rtc_month)();
int (get_rtc_year)();
int(rtc_subscribe_int)(uint8_t *bit_no) ;
int(rtc_unsubscribe_int)() ;
int (read_time)();
void (read_reg_B)();
void (write_reg_B)();
void (disable_interrupts_rtc)();
void (enable_interrupts_rtc)();
void (wait_valid_rtc)();

#endif
