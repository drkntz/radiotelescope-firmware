/* main.c
 * Main file for radio-telescope control system project
 * Authors: Zach Martin & Aaron Olsen
 * Date: 3/9/2023
 * 
 * TODO: 
 *  * init diags via some kind of passcode ('q' 3 times over usb?)
*/

////////////////////////////////////////////////////////////////////////////////
// Include other files.
#include <xc.h>
#include "common.h"     // This contains globals & every other header needed.
#include "diagnostic.h" // For testing hardware & software.

////////////////////////////////////////////////////////////////////////////////
// function prototypes.
void welcome_message(void);
void convert_angles(void);

////////////////////////////////////////////////////////////////////////////////
// Main program. Configure everything and update states in an endless loop.
int main(void)
{
    bool diags;
    
    SYSTEM_Initialize();
    
    diags = !DIAGS_GetValue();
    
    welcome_message();
    delay_ms(2000);     // Allow the message to display
        
    // Run diagnostic if button is pressed
    if(diags) diagnostic_main(); 
    
    // Initialize timestamp for blinking LED
    uint16_t led_time = timestamp_raw();
    
    // The superlooop
    while (1)
    {           
        /* From notes: this is the superloop
         * read PC commands
         * read buttons
         * update rotary encoders               done
         * update motors
         * send status at 3Hz (check notes)
         * refresh LCD                          done
         * blink status LEDs                    done
         */
        
        convert_angles(); // Convert rotary encoder pulses to angles
        
        refresh_lcd(); // Refresh LCD at 2Hz
        
        // Blink LED at 4Hz
        if(timestamp_to_ms(timestamp_raw() - led_time) > 250) 
        {
            nSTAT_LED_Toggle(); // "Heartbeat" LED
            led_time = timestamp_raw();
        }
        
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

// convert rotary encoder pulses to angles
void convert_angles(void)
{
    // Store angles, in tenths of a degree.
    // If we add up the pulses from both encoders and convert from the gear ratio,
    // we get 792.222 pulses per degree output. 
    // TODO: test this thoroughly. Pulses are stored in uint32_t, so we should
    // make sure the math is done correctly. 
    motor.az.degrees = (motor.az.pulse1 + motor.az.pulse2)/79.22;
    motor.alt.degrees = (motor.alt.pulse1 + motor.alt.pulse2)/79.22;
}