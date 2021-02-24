// LCD 16x2 Functions
// Adapted from https://electrosome.com/lcd-pic-mplab-xc8/

// define LCD_PORT as the data port of the display with matching endianess or
// define D0 to D7 as the pins of the data port
// RS, RW and EN as the respective pins of the lcd
// RW not yet used


#ifndef __lcd__
#define __lcd__

#define _XTAL_FREQ 4000000

#define LCD_PORT PORTD
#define RS       PORTEbits.RE0
#define EN       PORTEbits.RE1

#include <xc.h>
#include <stdint.h>

void lcd_init(void);

void lcd_cmd(uint8_t a);

void lcd_clear(void);

void lcd_move_cursor(uint8_t row, uint8_t col);

void lcd_write_char(uint8_t a);

void lcd_write_string(uint8_t *a);

void lcd_shift_right(void);

void lcd_shift_left(void);

void lcd_port(uint8_t a);

#endif //__lcd__