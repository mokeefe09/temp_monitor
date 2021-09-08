#ifndef	__LCD_H__
#define	__LCD_H__ 

#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "io_map.h"
#include "delay.h"

#define LCD_PORT_CLOCK	(1U << 1)
#define	LCD_PORT	GPIOB
#define LCD_DATA_BITS	(PIN4 | PIN5 | PIN6 | PIN7)	
#define EN_BIT		PIN3
#define RS_BIT		PIN2

// 0b00110000 - Data bits value for initialization sequence commands 
#define DISP_INIT	0x30

// 0b00100000 - Instruction to use 4 bit commands. Used in init sequence
#define DISP_4BITS	0x20

// 0b00101000 - Instruction to use 4 bit commands and 2-line display. Used
// in init sequence.
#define DISP_CONFIG	0x28

// 0b00001000 - Instruction to turn disp off. Used in init sequence. 
#define DISP_OFF	0x08

// 0b00000001 - Instruction to clear disp. Used in init sequence.
#define DISP_CLR	0x01

// 0b00000110 - Instruction that increments AC and disables shift. Used
// in init sequence.
#define DISP_EMS	0x06

// 0b00001100 - Instruction to turn display on. Used in init sequence.
#define DISP_ON		0x0C

// Number of rows and columns in LCD
#define LCD_LINES	2
#define LCD_WIDTH	16

// Addresses of first line: 	0x00 - 0x0F
// Addresses of second line: 	0x40 - 0x4F
#define DD_RAM_ADDR1	0x00
#define DD_RAM_ADDR2	0x40

void lcd_init(void);
void lcd_port_data(uint8_t d);
void lcd_strobe(void);
void lcd_send_cmd(uint8_t cmd);
void lcd_display(int x, int y, const char *str);
void lcd_putc(uint8_t c);

#endif
