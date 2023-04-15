/* main.c
 * Main file for radio-telescope control system project
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/9/2023
 * 
 * TODO: 
 *  * diagnostics via diags button and some kind of passcode ('q' 3 times over usb?)
*/

////////////////////////////////////////////////////////////////////////////////
// Include other files.
#include <xc.h>
#include "common.h"     // This contains globals & every other header needed
#include "diagnostic.h" // For testing hardware & software

////////////////////////////////////////////////////////////////////////////////
// function prototypes.
void welcome_message(void);

////////////////////////////////////////////////////////////////////////////////
// Main program. Configure everything and update states in an endless loop.
int main(void)
{
    SYSTEM_Initialize();
    
    welcome_message();
    delay_ms(2000); // Allow the message to display
        
    // Run diagnostic if button is pressed
    if(!DIAGS_GetValue()) diagnostic_main(); 
    
    // Initialize timestamp for blinking LED
    uint16_t led_time = timestamp_raw();
    
    // The superlooop
    while (1)
    {           
        /* From notes: this is the superloop
         * read PC commands
         * read buttons
         * update rotary encoders
         * update motors
         * send status at 3Hz (check notes)
         * refresh LCD
         * blink status LEDs
         */
        
        refresh_lcd(); // Refresh LCD at 10Hz
        
        // Blink LED at 4Hz
        if(timestamp_to_ms(timestamp_raw() - led_time) > 250) 
        {
            nSTAT_LED_Toggle(); // "Heartbeat" LED
            led_time = timestamp_raw();
        }
        
        //printf("\r\nstamp = %u", timestamp_ms());
        ClrWdt();
    }

    return 1;
}


void welcome_message(void)
{
    print_output = PRINT_TAG;
    printf("\r\nRadio Telescope Control System Begin\r\n");
    
    lcd_setCursor(0,0);

    print_output = PRINT_LCD;
    printf("Radio Telescope");
    lcd_setCursor(0,1);
    printf("Control System");
    print_output = PRINT_TAG;
}
