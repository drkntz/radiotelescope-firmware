/* Adapted from DFRobot Arduino code
 * Drive i2c display with PIC24
 * Zach Martin, 4/8/23
 */
 
#ifndef LiquidCrystal_PIC_h
#define LiquidCrystal_PIC_h
#include "common.h" // for common utilities and delays

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001  // Register select bit

// Public functions ////////////////////////////////////////////////////////////
void lcd_create(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows); // Use this to setup initially
void lcd_init();        // start device
void lcd_begin(uint8_t cols, uint8_t lines, uint8_t dotsize); // this is actually called in init, no need to call it explicitly? TODO: make private

/* Use these for high-level commands */
void lcd_clear();       // clear display & set cursor position to 0
void lcd_home();        // set cursor position to 0
void lcd_setCursor(uint8_t col, uint8_t row);   // set arbitrary cursor position
void lcd_noDisplay();   // turn off display
void lcd_display();     // turn on display
void lcd_noCursor();    // hide underline cursor
void lcd_cursor();      // display underline cursor
void lcd_noBlink();     // cursor does not blink
void lcd_blink();       // cursor will blink
void lcd_scrollDisplayLeft(void);   // scroll words
void lcd_scrollDisplayRight(void);   // scroll words
void lcd_leftToRight(void); // text that flows left to right
void lcd_rightToLeft(void); // text that flows right to left
void lcd_autoscroll(void);  // this will right justify text from cursor
void lcd_noAutoscroll(void);  // this will left justify text from cursor
void lcd_createChar(uint8_t location, uint8_t charmap[]);// Allows us to fill the first 8 CGRAM locations with custom characters
void lcd_noBacklight(void); // disable backlight
void lcd_backlight(void);    // enable backlight
void lcd_write(char *data, uint8_t len);//char *data, size_t len); // write a character array to lcd
void lcd_printf_write(char data); // this is used for printf writing to LCD display

#endif // LiquidCrystal_PIC_h
