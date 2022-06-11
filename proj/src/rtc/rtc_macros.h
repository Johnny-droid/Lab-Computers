#ifndef RTC_MACROS_H
#define RTC_MACROS_H

#define RTC_IRQ 8

// Two ports to access the internal registers
#define  RTC_ADDR_REG 0x70 // loaded with the RTC register address to access
#define RTC_DATA_REG 0x71 // to transfer the data to/from the RTC’s register in question


// Registers
#define SECONDS 0x00
#define MINUTES 0x02
#define HOURS 0x04
#define DAY_OF_THE_WEEK 0x06
#define DATE_OF_THE_MONTH 0x07
#define MONTH 0x08
#define YEAR 0x09
#define REGISTER_A 0x0A
#define REGISTER_B 0x0B
#define REGISTER_C 0x0C
#define REGISTER_D 0x0D

#define RTC_UIP      0x80   		/**< @brief Bit 7 of RTC's Register A */
#define disINT       0x8F			/**< @brief Bitmask to disable RTC interrupts */
#define enINT        0x70			/**< @brief Bitmask to enable RTC interrupts */
#define enPINT       0x40			/**< @brief Bitmask to enable RTC periodic interrupts */
#define set24        0x02			/**< @brief Bitmask to set 24 hours range */
#define setBCD       0xFB 			/**< @brief Bitmask to set data format to BCD */



#endif
