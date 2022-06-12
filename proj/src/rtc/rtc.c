#include "rtc.h"


/* GLOBAL VARIABLES */

unsigned char reg_A;
unsigned char reg_B;

static int g_rtc_hook_id = 8;			


void(rtc_ih)(){
  read_time();
  hours = get_rtc_hours();
  minutes = get_rtc_minutes();
}


int (get_rtc_seconds)()
{
	return g_rtc_seconds;
}
int (get_rtc_minutes)()
{
	return g_rtc_minutes;
}
int (get_rtc_hours)()
{
	return g_rtc_hours;
}
int (get_rtc_day)()
{
	return g_rtc_day;
}
int (get_rtc_month)()
{
	return g_rtc_month;
}
int (get_rtc_year)()
{
	return g_rtc_year;
}

int(rtc_subscribe_int)(uint8_t *bit_no) 
{
  *bit_no = g_rtc_hook_id = RTC_IRQ;
  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &g_rtc_hook_id);
} 


int(rtc_unsubscribe_int)() 
{
  return sys_irqrmpolicy(&g_rtc_hook_id);
}

int (read_time)(){

	wait_valid_rtc();
  sys_outb(RTC_ADDR_REG, SECONDS);
	util_sys_inb(RTC_DATA_REG, &g_rtc_seconds);
	g_rtc_seconds = to_BCD(g_rtc_seconds);

  sys_outb(RTC_ADDR_REG, MINUTES);
	util_sys_inb(RTC_DATA_REG, &g_rtc_minutes);
	g_rtc_minutes = to_BCD(g_rtc_minutes);

  sys_outb(RTC_ADDR_REG, HOURS);
	util_sys_inb(RTC_DATA_REG, & g_rtc_hours);
	g_rtc_hours = to_BCD(g_rtc_hours);

  sys_outb(RTC_ADDR_REG, DATE_OF_THE_MONTH);
	util_sys_inb(RTC_DATA_REG, &g_rtc_day);
	g_rtc_day = to_BCD(g_rtc_day);

  sys_outb(RTC_ADDR_REG, MONTH);
	util_sys_inb(RTC_DATA_REG, &g_rtc_month);
	g_rtc_month = to_BCD(g_rtc_month);

  sys_outb(RTC_ADDR_REG, YEAR);
	util_sys_inb(RTC_DATA_REG, &g_rtc_year);
	g_rtc_year = to_BCD(g_rtc_year);

	return 0;
}

void (read_reg_B)()
{
	 uint8_t reg_B1=0;

	//Read RTC's Register B and store it in a convenient variable
	sys_outb(RTC_ADDR_REG, REGISTER_B);
	util_sys_inb(RTC_DATA_REG, &reg_B1);
	reg_B = (unsigned char) reg_B1;
}
uint8_t (to_BCD)(uint8_t value)
{
	return ((value & 0xF0) >> 4) * 10 + (value & 0x0F);
}

void (write_reg_B)()
{
	sys_outb(RTC_ADDR_REG, REGISTER_B);
	sys_outb(RTC_DATA_REG, reg_B);
}

void (disable_interrupts_rtc)()
{
	read_reg_B();
	reg_B |= set24;
	reg_B &= setBCD;

	//Disable interrupts
	reg_B &= disINT;

	write_reg_B();

}

void (enable_interrupts_rtc)(){

	read_reg_B();

	//Enable interrupts
	reg_B |= enINT;

	write_reg_B();

}

void (wait_valid_rtc)()
{
	uint8_t reg_A1 = 0;

	do
	{
		disable_interrupts_rtc();
		sys_outb(RTC_ADDR_REG, REGISTER_A);
		util_sys_inb(RTC_DATA_REG, &reg_A1);
		enable_interrupts_rtc();
	} while (reg_A1 & RTC_UIP);

	reg_A = (unsigned char) reg_A1;
}
