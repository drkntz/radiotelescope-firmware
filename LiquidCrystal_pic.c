/* Adapted from DFRobot Arduino code
 * Drive 16x2 alphanumeric i2c display with PIC24
 *
 * Zach Martin, 4/8/23
 */

#include "LiquidCrystal_pic.h"
#include "mcc_generated_files/i2c1.h"

/* Local scope */ 
static void _send(uint8_t, uint8_t);
static void _write4bits(uint8_t);
static void _expanderWrite(uint8_t);
static void _pulseEnable(uint8_t);
static inline void _command(uint8_t value);
static void _lcd_begin(uint8_t cols, uint8_t lines, uint8_t dotsize);
static uint8_t _Addr;
static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;
static uint8_t _numlines;
static uint8_t _cols;
static uint8_t _rows;
static uint8_t _backlightval;


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
// The display must be reset whenever the mcu is reset or it may retain previous state

// Define the LCD size EG 16x2
void lcd_create(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

// global scope initialize
void lcd_init(){
	// if not already done in system init, you would put I2C1_Initialize() here.
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	_lcd_begin(_cols, _rows, 0);
}


static void _lcd_begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
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
	// before sending commands.
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

/* Send a character array to the LCD. 
 * We can also use printf() and its syntax by changing the write() function in uart2.c 
 * to point to the LCD instead of the UART. 
 * For example: 
 *
 * print_output = PRINT_LCD; // change global printf() setting
 * printf("This goes to LCD");
 * print_output = PRINT_TAG; 
 * printf("\r\nThis goes to tag-connect debug port");
 */
void lcd_write( char *data, uint8_t len)
{
    uint8_t i;
    
    for(i=0; i < len; i++)
    {
        _send(*(data + i), Rs);
    }
}

// this is used for printf formatting for the LCD display
void lcd_printf_write(char data)
{
    _send(data, Rs);
}

/*********** mid level commands, for sending data/cmds */
// Local scope
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

////////////////////////////////////////////////////////////////////////////////
// Functions specific to the project

// Update LCD based on motor status
// TODO: could use scrolldisplayleft functionality and display a scrolling message
// to give more flexibility.
uint8_t refresh_lcd(void)
{
    static uint16_t timestamp = 0;
    
    // This function runs at 2Hz. TODO: could define rate of calls in main()
    // instead of in each function and have groups of functions that run at x hz? or would that
    // create timing issues?. Or we could do this in a macro to keep it clean
    if(timestamp_to_ms(timestamp_raw() - timestamp) > 500) 
    {
        timestamp = timestamp_raw();
    }
    else return 0; // return early if it is not time to refresh
          
    uint8_t print_op_save = print_output;   // Store current state of printf.
    print_output = PRINT_LCD;   // Set printf to point to LCD. 
    lcd_setCursor(0,0);     // Start at the top row
    
    /* For now, the top row is system status.
     * It displays if the device is running OK and which motor direction is 
     * enabled. Example message: "STAT: PC AZ+ EL-"
     */
    
    
    switch(command.source)
    {   
        //////////////////////////////////////////////////////////////////////////
        case CMD_SRC_PC:
            lcd_setCursor(13, 0);
            printf(" PC");
            lcd_setCursor(0,0);
            
            //////////////////
            printf("STAT:");
            printf(" AZ");
            // Azimuth direction
            switch(motor.az.dir)
            {
                case MOTOR_POS:
                    printf("+");
                    break;
                case MOTOR_NEG:
                    printf("-");
                    break;
                default: // aka MOTOR_STOP
                    printf("0");
                    break;
            }
            ///////
            // Elevation direction
            printf(" EL");
            switch(motor.alt.dir)
            {
                case MOTOR_POS:
                    printf("+");
                    break;
                case MOTOR_NEG:
                    printf("-");
                    break;
                default: // aka MOTOR_STOP
                    printf("0");
                    break;
            }
            // Update bottom row
            lcd_setCursor(0,1);     // Write the bottom row

            /* The bottom row is the degrees of the motor system.
             * The message it prints is "AZ 123.4 EL 12.4"
             * where 123.4 and 12.4 are the azimuth and elevation degrees.
             * Since the elevation degrees shouldn't go above 90, 
             * we can more or less assume we won't print outside the LCD boundary.
             */
            printf("AZ %3u.%1u EL %2u.%1u", motor.az.degrees/10, motor.az.degrees%10,
                    motor.alt.degrees/10, motor.alt.degrees%10); 
            break;
            //////////////////////////////////////////////////////////////////////////
        case CMD_SRC_LOCAL:
            lcd_setCursor(14, 0);
            printf(" LC");
            lcd_setCursor(0,0);
            // Cases for the current menu operation at local control 
            switch(local_menu_state)
            {
                case 1: // Azimuth motor control
                    printf("+ Az -");
                    break;
                case 2:
                    printf("+ Alt -");
                    break;
                case 3:
                    printf("Az Reset-0 El");
                    break;
                case 4:
                    printf("  Quit OK");
                    break;
            }
            lcd_setCursor(0,1);
            /* The bottom row is the degrees of the motor system.
             * The message it prints is "AZ 123.4 EL 12.4"
             * where 123.4 and 12.4 are the azimuth and elevation degrees.
             * Since the elevation degrees shouldn't go above 90, 
             * we can more or less assume we won't print outside the LCD boundary.
             */
            printf("AZ %3u.%1u EL %2u.%1u", motor.az.degrees/10, motor.az.degrees%10,
                    motor.alt.degrees/10, motor.alt.degrees%10); 
            break;
            //////////////////////////////////////////////////////////////////////////
        case CMD_SRC_DEBUG:
            lcd_setCursor(14, 0);
            printf(" DB");
            lcd_setCursor(0,0);
            
            //////////////////
            printf("STAT:");
            printf(" AZ");
            // Azimuth direction
            switch(motor.az.dir)
            {
                case MOTOR_POS:
                    printf("+");
                    break;
                case MOTOR_NEG:
                    printf("-");
                    break;
                default: // aka MOTOR_STOP
                    printf("0");
                    break;
            }
            ///////
            // Elevation direction
            printf(" EL");
            switch(motor.alt.dir)
            {
                case MOTOR_POS:
                    printf("+");
                    break;
                case MOTOR_NEG:
                    printf("-");
                    break;
                default: // aka MOTOR_STOP
                    printf("0");
                    break;
            }
            // Update bottom row
            lcd_setCursor(0,1);     // Write the bottom row

            /* The bottom row is the degrees of the motor system.
             * The message it prints is "AZ 123.4 EL 12.4"
             * where 123.4 and 12.4 are the azimuth and elevation degrees.
             * Since the elevation degrees shouldn't go above 90, 
             * we can more or less assume we won't print outside the LCD boundary.
             */
            printf("AZ %3u.%1u EL %2u.%1u", motor.az.degrees/10, motor.az.degrees%10,
                    motor.alt.degrees/10, motor.alt.degrees%10); 
            break;
            //////////////////////////////////////////////////////////////////////////
        default:
            printf("ERR            "); // TODO
            lcd_setCursor(0,1);
            printf("               ");
            break;
    }
    

    print_output = print_op_save; // Restore state of printf
    return 1;
}