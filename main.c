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
void process_command(void);
void update_motors(void);

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
         * process commands                     mostly done
         * update motors                        done
         * send status at 3Hz (check notes)
         * refresh LCD                          done
         * blink status LEDs                    done
         */
        
        convert_angles(); // Convert rotary encoder pulses to angles
        
        process_command();
        
        //if ((motor.alt.dir != prev_alt_dir) || (motor.az.dir != prev_az_dir))
        update_motors();  // update motor directions
        
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

void process_command(void)
{
    // This is so anything can instantly stop the az and alt motors
    if (command.command == CMD_STOP) // Turn off az and alt motors
    {
        AZ_CONTROL1_SetLow();
        AZ_CONTROL2_SetLow();
        EL_CONTROL1_SetLow();
        EL_CONTROL2_SetLow();
        motor.alt.dir = MOTOR_STOP;
        motor.az.dir = MOTOR_STOP;
    }
    
    // Go to a designated location //TODO - how to enter something like az = 46, alt = 25)
    //if (command.command == CMD_GOTO)
    //{
        // TODO make this USB connection, not TAG communication
        
    //}
    
    // CMD_REPORT_STATUS ----------TODO
    
    
    // CMD_HOME
    if (command.command == CMD_HOME)
    {
        // Establish target location to home (0,0) -- TODO this should be part of the PC input side of things then
        command.alt_deg = 0;
        command.az_deg = 0;
        
        // Determine alt/az directions --- TODO current setup may have problems with repeatedly going back and forth
        // Altitude
        if (motor.alt.degrees == command.alt_deg)
        {
            motor.alt.dir = MOTOR_STOP;
        }
        else if (motor.alt.degrees > command.alt_deg)
        {
            motor.alt.dir = MOTOR_NEG;
        }
        else if (motor.alt.degrees < command.alt_deg)
        {
            motor.alt.dir = MOTOR_POS;
        }
        // Azimuth
        if (motor.az.degrees == command.az_deg)
        {
            motor.az.dir = MOTOR_STOP;
        }
        else if (motor.az.degrees > command.az_deg)
        {
            motor.az.dir = MOTOR_NEG;
        }
        else if (motor.az.degrees < command.az_deg)
        {
            motor.az.dir = MOTOR_POS;
        }
        
        // When at location, change command to CMD_STOP
        if ((motor.alt.dir == MOTOR_STOP) && (motor.az.dir == MOTOR_STOP))
        {
            command.command = CMD_STOP;
        }
    }
    
    // Altitude
    // CMD_ALT_STOP
    if (command.command == CMD_ALT_STOP)
    {
        motor.alt.dir = MOTOR_STOP;
    }
    // CMD_ALT_POS - up
    else if (command.command == CMD_ALT_POS)
    {
        motor.alt.dir = MOTOR_POS;
    }
    // CMD_ALT_NEG - down
    else if (command.command == CMD_ALT_NEG)
    {
        motor.alt.dir = MOTOR_NEG;
    }
    // CMD_ALT_RESET - reset alt encoders to zero
    else if (command.command == CMD_ALT_RESET)
    {
        motor.alt.pulse1 = 0;
        motor.alt.pulse2 = 0;
    }

    // Azimuth
    // CMD_AZ_STOP
    if (command.command == CMD_AZ_STOP)
    {
        motor.az.dir = MOTOR_STOP;
    }
    // CMD_AZ_POS - Clockwise
    else if (command.command == CMD_AZ_POS)
    {
        motor.az.dir = MOTOR_POS;
    }
    // CMD_AZ_NEG - Counter-Clockwise
    else if (command.command == CMD_AZ_NEG)
    {
        motor.az.dir = MOTOR_NEG;
    }
    // CMD_AZ_RESET - reset az encoders to zero
    else if (command.command == CMD_AZ_RESET)
    {
        motor.az.pulse1 = 0;
        motor.az.pulse2 = 0;
    }
}

// Update the direction of the motors
void update_motors(void)
{
    // Stop Motors
    if (motor.alt.dir == MOTOR_STOP)
    {
        AZ_CONTROL1_SetLow();
        AZ_CONTROL2_SetLow();
    }
    if (motor.az.dir == MOTOR_STOP)
    {
        EL_CONTROL1_SetLow();
        EL_CONTROL2_SetLow();
    }
    
    // Altitude
    if (motor.alt.dir == MOTOR_POS) // Up
    {
        EL_CONTROL2_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        EL_CONTROL1_SetHigh();
    }
    else if (motor.alt.dir == MOTOR_NEG) // Down
    {
        EL_CONTROL1_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        EL_CONTROL2_SetHigh();
    }
    
    // Azimuth
    if (motor.az.dir == MOTOR_POS) // Clockwise
    {
        AZ_CONTROL2_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        AZ_CONTROL1_SetHigh();
    }
    else if (motor.az.dir == MOTOR_NEG) // Counter-Clockwise
    {
        AZ_CONTROL1_SetLow();
        __delay_ms(600);    // 600 ms is the motor turnoff time -- change this later to not delay whole program (use timer1)
        AZ_CONTROL2_SetHigh();
    }
}