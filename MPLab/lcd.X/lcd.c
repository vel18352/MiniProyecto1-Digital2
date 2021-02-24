// LCD 16x2 Functions
// Adapted from https://electrosome.com/lcd-pic-mplab-xc8/

// TODO: implement rest of supported features
// TODO: function for enable/disable cursor and blink
#include "lcd.h"

void lcd_init()
{
    lcd_port(0x00);
    __delay_ms(20);

    lcd_cmd(0x30);
    __delay_ms(5);

    lcd_cmd(0x30);
    __delay_ms(11);

    lcd_cmd(0x30);
    /////////////////////////////////////////////////////
    lcd_cmd(0x38);
    lcd_cmd(0x10);
    lcd_cmd(0x01);
    lcd_cmd(0x06);
    lcd_cmd(0x0F); // display on, cursor on, blink on
}

void lcd_port(uint8_t a)
{
    #ifdef LCD_PORT
    LCD_PORT = a;
    #endif

    #ifndef LCD_PORT
    D0 = (a &   1)? 1 : 0;
    D1 = (a &   2)? 1 : 0;
    D2 = (a &   4)? 1 : 0;
    D3 = (a &   8)? 1 : 0;
    D4 = (a &  16)? 1 : 0;
    D5 = (a &  32)? 1 : 0;
    D6 = (a &  64)? 1 : 0;
    D7 = (a & 128)? 1 : 0;
    #endif
}

void lcd_cmd(uint8_t cmd)
{
    RS = 0;             // => RS = 0
    lcd_port(cmd);

    EN  = 1;             // => E = 1
    __delay_ms(4);
    EN  = 0;             // => E = 0
}

void lcd_clear(void)
{
    lcd_cmd(0);
    lcd_cmd(1);
}

// set row and column of the cursor
void lcd_move_cursor(uint8_t row, uint8_t col)
{
    if(row == 0)
    {
        lcd_cmd(0x80 + col);
    }
    else if(row == 1)
    {
        lcd_cmd(0xC0 + col);
    }
}

void lcd_write_char(uint8_t data)
{
    RS = 1;             // => RS = 1
    lcd_port(data);

    EN  = 1;             // => E = 1
    __delay_ms(4);
    EN  = 0;             // => E = 0
}

void lcd_write_string(uint8_t *a)
{
    for(int i=0; a[i] != '\0'; i++)
    {
       lcd_write_char(a[i]);
    }
}

void lcd_shift_right()
{
    lcd_cmd(0x1C);
}

void lcd_shift_left()
{
    lcd_cmd(0x18);
}
