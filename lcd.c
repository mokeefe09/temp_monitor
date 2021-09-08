/*
Author: Michael O'Keefe
Description: Driver to interface a 16x2 HD44780-style LCD with the
	     TM4c123GH6PM mcu from Texas Instruments.

CITATION:
This driver was adapted from code posted in the article "Introduction to
Microcontrollers - Adding Some Real-World Hardware" on embeddedrelated.com,
written by Mike Silva: https://www.embeddedrelated.com/showarticle/490.php


*/

#include "lcd.h"

void lcd_init(void){

// Intialization of GPIO pins

	// Enable clock to the LCD GPIO port
	SYSCTL->RCGCGPIO	|=	LCD_PORT_CLOCK;

	// Set direction of LCD pins to output
	LCD_PORT->DIR		|=	(LCD_DATA_BITS | EN_BIT | RS_BIT);

	// Disable alternate functions of LCD pins. Will use as simple GPIO pins
	LCD_PORT->AFSEL 	&= 	~(LCD_DATA_BITS | EN_BIT | RS_BIT);

	// Enable digital functions of LCD pins
	LCD_PORT->DEN		|=	(LCD_DATA_BITS | EN_BIT | RS_BIT);
	
	// Mask interrupts to prevent unwanted interrupts
	LCD_PORT->IM		&=	~(LCD_DATA_BITS | EN_BIT | RS_BIT);

	// Clear enable bit and R/W bit before beginning LCD init sequence
	LCD_PORT->DATA		&= 	~(EN_BIT | RS_BIT);


// Initialization of LCD device. Must send the same 4-bit sequence, 0011, to the
// lcd 3 times, with specific delay periods in-between.

	// Obligatory delay before initalization
	ms_delay(15);

	// Set the data bits to the initialization data value
	lcd_port_data(DISP_INIT);

	// Send data
	lcd_strobe();

	// Delay must be greater than 4.1ms
	ms_delay(5);
	
	// Set the data bits to the initialization data value
	lcd_port_data(DISP_INIT);

	// Send data
	lcd_strobe();

	// Delay must be greater than 100 microseconds
	ms_delay(1);

	// Set the data bits to the initialization data value
	lcd_port_data(DISP_INIT);

	// Send data
	lcd_strobe();

	// Delay must be greater than 100 microseconds
	ms_delay(1);
	
	// Only will use 4 data lines to transmit data
	lcd_port_data(DISP_4BITS);
	lcd_strobe();
	ms_delay(1);

	// Use 4 data lines AND a 2-line display (16x2)
	lcd_send_cmd(DISP_CONFIG);

	// Turn display off
	lcd_send_cmd(DISP_OFF);

	// Clear display
	lcd_send_cmd(DISP_CLR);

	// Extra delay needed after clear display instruction (min 1.52ms delay)
	ms_delay(2);
	
	// Increment address counter to the right and disable shift
	lcd_send_cmd(DISP_EMS);

	// Turn display on
	lcd_send_cmd(DISP_ON);
}

void lcd_port_data(uint8_t d){

	LCD_PORT->DATA = (LCD_PORT->DATA & ~LCD_DATA_BITS) | (d & 0xF0);
}

void lcd_strobe(void){
	// Set the enable bit, which will send the 4 bits of data
	// on the gpio data bits to the lcd.
	LCD_PORT->DATA	|=	EN_BIT;

	// Delay only has to be 150ns, but we will just use ms delay b/c we
	// do not have strict timing constraints.
	ms_delay(1);

	// Clear the enable bit
	LCD_PORT->DATA	&=	~EN_BIT;

	// Delay after clearing enable bit delay would only have to be 10ns
	ms_delay(1);
}

void lcd_send_cmd(uint8_t cmd){
	// Since we are sending an instruction into the instruction register of
	// the LCD, we must set the RS bit low. Setting the RS bit high would
	// result in a write to the data register of the LCD.
	LCD_PORT->DATA	&=	~RS_BIT;

	// Send high 4 bits of data
	lcd_port_data(cmd & 0xF0);
	lcd_strobe();

	// Send low 4 bits of data
	lcd_port_data((cmd & 0xF) << 4);
	lcd_strobe();

	// A 1ms delay covers all instructions except clear display and return home
	ms_delay(1);

}

void lcd_display(int x, int y, const char *str){
	int n = LCD_WIDTH - x;
	uint8_t addr;

	// Check if y is out of bounds
	if ((y < 0) || (y >= LCD_LINES)){
		return;
	}
	
	switch (y){
		case 0:
			// We will be writing to line 1 of the lcd
			addr = DD_RAM_ADDR1;
			break;

		case 1:
			// We will be writing to line 2 of the lcd
			addr = DD_RAM_ADDR2;
			break;

		default:
			// Should never have to be executed
			return;
	}

	// Send instruction to change the address of the address counter register in
	// the lcd to the address that we will start writing to.
	lcd_send_cmd(addr + x + 0x80);

	// iterate through the string and send each character one at a time.
	while (*str && n--){
		lcd_putc(*str);
		str++;
	}

}

void lcd_putc(uint8_t c){
	// Set the RS bit b/c we will be writing to the data register of the lcd
	LCD_PORT->DATA	|=	RS_BIT;

	// Send high 4 bits
	lcd_port_data(c & 0xf0);
	lcd_strobe();

	// Send low 4 bits
	lcd_port_data((c << 4) & 0xF0);
	lcd_strobe();
}
