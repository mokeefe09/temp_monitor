/*
Author: Michael O'Keefe
Date: 09/13/2021
Description: Driver to interface a 16x2 HD44780-style LCD with the
	     TM4c123GH6PM mcu from Texas Instruments. See Sitronix ST0766
	     Dot Matrix LCD Controller/Driver datasheet for details on 
	     LCD operation.

CITATION:
This driver was adapted from code posted in the article "Introduction to
Microcontrollers - Adding Some Real-World Hardware" on embeddedrelated.com,
written by Mike Silva: https://www.embeddedrelated.com/showarticle/490.php
*/

#include "lcd.h"

void lcd_init(void){
/*
Initializes the LCD to a known state from any state. At the end of this sequence
the LCD is on and cleared, ready to receive instructions. Exclusively uses the
GPIOB port for the data bits, EN bit, and RS bit. The R/W bit is tied to ground
it hardware because we will exclusively be writing to the LCD.
*/

// Intialization of GPIO pins

	// Enable clock to the GPIOB port
	SYSCTL->RCGCGPIO	|=	(1U << 1);

	// Set direction of LCD pins to output
	GPIOB->DIR		|=	(LCD_DATA_BITS | EN_BIT | RS_BIT);

	// Disable alternate functions of LCD pins. Will use as simple GPIO pins
	GPIOB->AFSEL 		&= 	~(LCD_DATA_BITS | EN_BIT | RS_BIT);

	// Enable digital functions of LCD pins
	GPIOB->DEN		|=	(LCD_DATA_BITS | EN_BIT | RS_BIT);
	
	// Mask interrupts to prevent unwanted interrupts
	GPIOB->IM		&=	~(LCD_DATA_BITS | EN_BIT | RS_BIT);

	// Clear enable bit and R/W bit before beginning LCD init sequence
	GPIOB->DATA		&= 	~(EN_BIT | RS_BIT);


// Initialization of LCD device. Must send the same 4-bit sequence, 0011, to the
// lcd 3 times, with specific delay periods in-between. See datasheet for detailed
// steps of initialization sequence: Sitronix ST7066 Dot Matrix LCD Controller/Driver

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
	tiny_delay(200);

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
/*
Prepares data to be sent to the LCD by setting the 4 GPIO data pins
connected to the LCD data lines to the upper 4 bits of the input parameter, d.
Note, the lower four bits of the GPIO data register are not affected by this
operation.
*/
	// First, clear the LCD data bits. Then, OR them with the high 4 bits
	// of the input parameter, d. This is the data that will be sent to the
	// LCD.
	GPIOB->DATA = (GPIOB->DATA & ~LCD_DATA_BITS) | (d & 0xF0);
}

void lcd_strobe(void){
/*
Sends data from the GPIO data pins to the LCD by setting the EN bit. 
*/
	// Set the enable bit, which will send the 4 bits of data
	// on the gpio data bits to the lcd.
	GPIOB->DATA	|=	EN_BIT;

	// Delay only has to be 150ns. 
	nano_delay();

	// Clear the enable bit
	GPIOB->DATA	&=	~EN_BIT;

	// Delay after clearing enable bit delay would only have to be 10ns
	nano_delay();
}

void lcd_send_cmd(uint8_t cmd){
/*
Sends a command to the LCD. We are in 4-bit mode, so we must set the data register
twice and send two 4-bit transmissions. The high 4 bits of the 8 bit instruction
are sent first.
*/
	// Since we are sending an instruction into the instruction register of
	// the LCD, we must set the RS bit low. Setting the RS bit high would
	// result in a write to the data register of the LCD. Requires 30ns of
	// delay before we set the enable bit, but since we will call the lcd_port_data
	// function before setting the enable bit in lcd_strobe(), we will definitely
	// have enough of a delay.
	GPIOB->DATA	&=	~RS_BIT;

	// Send high 4 bits of data
	lcd_port_data(cmd & 0xF0);
	lcd_strobe();

	// Send low 4 bits of data
	lcd_port_data((cmd & 0xF) << 4);
	lcd_strobe();

	// A 50us delay covers all instructions except clear display and return home
	tiny_delay(200);

}

void lcd_display(int x, int y, const char *str){
/*
Displays a string on the 16x2 LCD screen.

Receives:	int x: The horizontal (x-coord) position the message will start from. Can
		be in the interval [0, 15].

		int y: The row the message will start on. For the 1st row, input 0. For
		the 2nd row, input 1. There are only 2 rows on the screen.

		const char *str: Pointer to the string that will be displayed.

Returns: 	Nothing
*/
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
/*
Writes a character to the LCD. Note, when we are sending instructions the RS bit is
low. When we write data with this function, we set the RS bit high. Since we are in
4-bit mode, we have to set the GPIO data and send it twice, with the upper 4 bits
being sent first.
*/
	// Set the RS bit b/c we will be writing to the data register of the lcd
	GPIOB->DATA	|=	RS_BIT;

	// Send high 4 bits
	lcd_port_data(c & 0xf0);
	lcd_strobe();

	// Send low 4 bits
	lcd_port_data((c << 4) & 0xF0);
	lcd_strobe();
}
