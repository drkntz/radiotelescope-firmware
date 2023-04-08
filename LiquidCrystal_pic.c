/* Adapted from DFRobot Arduino code
 * Drive i2c display with PIC24
 * Zach Martin, 4/8/23
 */

// TODO: deal with these includes
#include "LiquidCrystal_pic.h"
#include "mcc_generated_files/i2c1.h"

// TODO: do I have to use "static" for variables as well as local scope functions?
/* Local scope */ 
static void _init_priv();
static void _send(uint8_t, uint8_t);
static void _write4bits(uint8_t);
static void _expanderWrite(uint8_t);
static void _pulseEnable(uint8_t);
static inline void _command(uint8_t value);
static uint8_t _Addr;
static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;
static uint8_t _numlines;
static uint8_t _cols;
static uint8_t _rows;
static uint8_t _backlightval;


//// TODO:
//#define printIIC(args)	Wire.send(args)
//inline void LiquidCrystal_I2C::write(uint8_t value) {
//	send(value, Rs);
//}



// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

// this replaces the object declaration in C++
void lcd_create(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

// global scope initialize
void lcd_init(){
	_init_priv();
}

static void _init_priv()
{
	// if not already done in system init, you would put I2C1_Initialize() here.
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	lcd_begin(_cols, _rows, 0);  // TODO: I just set dotsize to zero for now. don't think we need to or can do that 10dot high thing 
}

void lcd_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	__delay_ms(50); 
  
	// Now we pull both RS and R/W low to begin commands
	_expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	__delay_ms(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
    // we start in 8bit mode, try to set 4 bit mode
   _write4bits(0x03 << 4);
   __delay_us(4500); // wait min 4.1ms
   
   // second try
   _write4bits(0x03 << 4);
   __delay_us(4500); // wait min 4.1ms
   
   // third go!
   _write4bits(0x03 << 4); 
   __delay_us(150);
   
   // finally, set to 4-bit interface
   _write4bits(0x02 << 4); 


	// set # lines, font size, etc.
	_command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_display();
	
	// clear it off
	lcd_clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	_command(LCD_ENTRYMODESET | _displaymode);
	
	lcd_home();
}

/********** high level commands, for the user! */
void lcd_clear(){
	_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	__delay_ms(2);  // this command takes a long time!
}

void lcd_home(){
	_command(LCD_RETURNHOME);  // set cursor position to zero
	__delay_ms(2);  // this command takes a long time!
}

void lcd_setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcd_noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_display() {
	_displaycontrol |= LCD_DISPLAYON;
	_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_cursor() {
	_displaycontrol |= LCD_CURSORON;
	_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_blink() {
	_displaycontrol |= LCD_BLINKON;
	_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(void) {
	_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_scrollDisplayRight(void) {
	_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	_command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		_send(charmap[i], Rs);
	}
}

// Turn the (optional) backlight off/on
void lcd_noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	_expanderWrite(0);
}

void lcd_backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	_expanderWrite(0);
}

// TODO: this is a cheap way to avoid using the arduino write libraries
// there may be something about register select? send RS??? don't know
void lcd_write(char *data, size_t len)
{
    I2C1_MESSAGE_STATUS status;
    uint8_t timeout = 100;
    
    I2C1_MasterWrite(data, len, _Addr, &status);
    
    while(status == I2C1_MESSAGE_PENDING && timeout > 0)
    {
        timeout--;
        __delay_ms(1);
    }
    if(status == I2C1_MESSAGE_COMPLETE) // success!
    {
        // TODO: could throw some flag indicating success or failure
        return;
    } 
}


/*********** mid level commands, for sending data/cmds */

static inline void _command(uint8_t value) {
	_send(value, 0);
}


/************ low level data pushing commands **********/
// local scope
// write either command or data
void _send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    _write4bits((highnib)|mode);
	_write4bits((lownib)|mode); 
}

void _write4bits(uint8_t value) {
	_expanderWrite(value);
	_pulseEnable(value);
}

// write to expander using I2C1 driver
void _expanderWrite(uint8_t _data)
{ 
    I2C1_MESSAGE_STATUS status;
    uint8_t timeout = 100;      // TODO: wait 100ms for display to respond
    uint8_t newdata = _data | _backlightval;
    
    I2C1_MasterWrite(&newdata, 1, _Addr, &status);
    
    while(status == I2C1_MESSAGE_PENDING && timeout > 0)
    {
        timeout--;
        __delay_ms(1);
    }
    if(status == I2C1_MESSAGE_COMPLETE) // success!
    {
        // TODO: could throw some flag indicating success or failure
        return;
    } 
}

void _pulseEnable(uint8_t _data){
	_expanderWrite(_data | En);	// En high
	__delay_us(1);		// enable pulse must be >450ns
	
	_expanderWrite(_data & ~En);	// En low
	__delay_us(50);		// commands need > 37us to settle
} 