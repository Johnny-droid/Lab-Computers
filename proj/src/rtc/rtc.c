#include "rtc.h"
#include <stdio.h>
#include <minix/driver.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <time.h> 


/* GLOBAL VARIABLES */

unsigned char reg_A;
unsigned char reg_B;
unsigned char g_rtc_seconds;
unsigned char g_rtc_minutes;
unsigned char g_rtc_hours;
unsigned char g_rtc_day;
unsigned char g_rtc_month;
unsigned char g_rtc_year;
static int g_rtc_hook_id = 8;				/**< @brief  RTC's Hook ID */


int(rtc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = g_rtc_hook_id = RTC_IRQ;
  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &g_rtc_hook_id);
} 


int(rtc_unsubscribe_int)() {
  return sys_irqrmpolicy(&g_rtc_hook_id);
}

Time (read_time)(){

	wait_valid_rtc();


  sys_outb(RTC_ADDR_REG, SECONDS);
	sys_inb(RTC_DATA_REG, &g_rtc_seconds);
  sys_outb(RTC_ADDR_REG, MINUTES);
	sys_inb(RTC_DATA_REG, &g_rtc_minutes);
  sys_outb(RTC_ADDR_REG, HOURS);
	sys_inb(RTC_DATA_REG, & g_rtc_hours);
  sys_outb(RTC_ADDR_REG, DATE_OF_THE_MONTH);
	sys_inb(RTC_DATA_REG, &g_rtc_day);
  sys_outb(RTC_ADDR_REG, MONTH);
	sys_inb(RTC_DATA_REG, &g_rtc_month);
  sys_outb(RTC_ADDR_REG, YEAR);
	sys_inb(RTC_DATA_REG, &g_rtc_year);



	//Store the already converted values in convenient variables
	unsigned int sec = interpret_BCD(g_rtc_seconds);
	unsigned int min = interpret_BCD(g_rtc_minutes);
	unsigned int hour = interpret_BCD(g_rtc_hours);
	unsigned int day = interpret_BCD(g_rtc_day);
	unsigned int month = interpret_BCD(g_rtc_month);
	unsigned int year = interpret_BCD(g_rtc_year);

	//Create a time object
	Time time = create_time(sec, min, hour, day, month, year);
   struct tm strtime;
    time_t timeoftheday;
 
    strtime.tm_year = g_rtc_year-1900;
    strtime.tm_mon = 1;
    strtime.tm_mday = g_rtc_day;
    strtime.tm_hour = g_rtc_hours;
    strtime.tm_min = g_rtc_minutes;
    strtime.tm_sec = g_rtc_seconds;
    strtime.tm_isdst = 0;
 
    timeoftheday = mktime(&strtime);
    printf(ctime(&timeoftheday));
 
	return time;
}

void read_reg_B()
{
	unsigned long reg_B1=0;

	//Read RTC's Register B and store it in a convenient variable
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_inb(RTC_DATA_REG, &reg_B1);
	reg_B = (unsigned char) reg_B1;
}

void write_reg_B()
{
	sys_outb(RTC_ADDR_REG, RTC_REG_B);
	sys_outb(RTC_DATA_REG, reg_B);
}

void disable_interrupts()
{
	read_reg_B();

	//Disable interrupts
	reg_B &= disINT;

	write_reg_B();

}

void enable_interrupts(){

	read_reg_B();

	//Enable interrupts
	reg_B |= enINT;

	write_reg_B();

}

void wait_valid_rtc()
{
	unsigned long reg_A1 = 0;

	do
	{
		disable_interrupts();
		sys_outb(RTC_ADDR_REG, RTC_REG_A);
		sys_inb(RTC_DATA_REG, &reg_A1);
		enable_interrupts();
	} while (reg_A1 & RTC_UIP);

	reg_A = (unsigned char) reg_A1;
}

