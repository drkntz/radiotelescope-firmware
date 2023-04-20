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
void print_usb_option_screen(void);
void read_pc_commands_temp(void);
void read_buttons(void);


motor_dir_t prev_alt_dir = MOTOR_STOP; // To use in the main loop (determine if update is necessary)
motor_dir_t prev_az_dir = MOTOR_STOP;

// Variable declarations for read_buttons()
uint16_t button_time = 0;
uint16_t button_delay = 250;

////////////////////////////////////////////////////////////////////////////////
// Main program. Configure everything and update states in an endless loop.
int main(void)
{
    bool diags;
    
    SYSTEM_Initialize();
    
    // TODO: clean up
    motor.alt.speed = 3;
    motor.az.speed = 3;
    motor.alt.pwm = 2;
    motor.az.pwm = 2;
    
    diags = !DIAGS_GetValue();
    
    welcome_message();
    delay_ms(2000);     // Allow the message to display
        
    // Run diagnostic if button is pressed
    if(diags) diagnostic_main(); 
    
    command.source = CMD_SRC_PC;
    
    // Initialize timestamp for blinking LED
    uint16_t led_time = timestamp_raw();
    
    // Initialize button_time timestamp
    button_time = timestamp_raw();
    
    // Print the USB Option Menu
    print_usb_option_screen();
    
    
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
        
        // Update to previous motor directions
        
        printf("\r Alt_Dir %x - Az_Dir %x - Alt_Deg %3d.%01u - Az_Deg %3d.%01u - Command   %u",
                motor.alt.dir, motor.az.dir, motor.alt.degrees/10,
                abs(motor.alt.degrees%10), motor.az.degrees/10, abs(motor.az.degrees%10),
                command.command);
        
        // Temporary read PC commands (this will be changed to interact with other code)
        read_pc_commands_temp();
        
        // Read buttons
        read_buttons();
        
        
        convert_angles(); // Convert rotary encoder pulses to angles
        
        process_command();
        
        // Determine if motor update is necessary
        if (!((motor.alt.dir == prev_alt_dir) && (motor.az.dir == prev_az_dir)))
        {
            update_motors();  // update motor directions
        }
        ///////
        
        refresh_lcd(); // Refresh LCD at 2Hz
        
        // Blink LED at 4Hz
        if(timestamp_to_ms(timestamp_raw() - led_time) > 250) // 250 ms
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
    print_output = PRINT_USB;
    printf("\r\n\nRadio Telescope Control System\r\n");
    
    lcd_setCursor(0,0);

    print_output = PRINT_LCD;
    printf("Radio Telescope");
    lcd_setCursor(0,1);
    printf("Control System");
    print_output = PRINT_USB;
}

void print_usb_option_screen(void) // UART1 options screen
{
    uint8_t print_op_save = print_output;   // Store current state of printf.
    print_output = PRINT_USB;
    printf("\r\n\n\nRadio Telescope Option Menu\r\nOptions:");
    printf( "\r\n1 - Stop Motors (CMD_STOP)"
            "\r\n2 - Altitude Up (CMD_ALT_POS)"
            "\r\n3 - Altitude Down (CMD_ALT_NEG)"
            "\r\n4 - Altitude Stop Motor (CMD_ALT_STOP)"
            "\r\n5 - Set Altitude Encoder to Zero Point (CMD_ALT_RESET)"
            "\r\n6 - Azimuth Clockwise (CMD_AZ_POS)"
            "\r\n7 - Azimuth Counter-Clockwise (CMD_AZ_NEG)"
            "\r\n8 - Azimuth Stop Motor (CMD_AZ_STOP)"
            "\r\n9 - Set Azimuth Encoder to Zero Point (CMD_AZ_RESET)"
            "\r\n0 - Take Motors to Home (0,0) Point (CMD_HOME)"
            "\r\nESC - Reprint the menu"
            "\r\nEnter option: ");
    print_output = print_op_save; // Restore state of printf
}

////////////////////////////////////////////////////////////////////////////////
// Read buttons
// Button 1 is left (down)   button
// Button 2 is center (menu) button
// Button 3 is right (up/OK) button
#define BUTTON_DEBOUNCE 250

// Update & debounce buttons
void read_buttons(void)
{
    static uint16_t b2_time = 0;
    
    static bool b2_prev = false; // button 2 triggers only on positive edge
    bool b1 = BUTTON1_GetValue();
    bool b2 = false;
    bool b3 = BUTTON3_GetValue();
    uint16_t raw_time;
    
    raw_time = timestamp_raw();
    
    // Debounce buttons
    // button 2 happens only on positive edge
    if(timestamp_to_ms(raw_time - b2_time) > BUTTON_DEBOUNCE && BUTTON2_GetValue() && !b2_prev)
    {
        b2 = true;
        b2_time = raw_time;
        command.command = CMD_STOP; // This fixes the bug where motor command is retained after switching to local control override
    }
    b2_prev = BUTTON2_GetValue();
    
    
    // if any button is pressed, change source to local control
    if ( b2 && local_menu_state == 0)
    {
        command.source = CMD_SRC_LOCAL; // set to local control source for commands
        local_menu_state = 1;
        return;
    }
    
    
    // Check for menu increment (mode button)
    if (b2 &&  (command.source == CMD_SRC_LOCAL))
    {
        local_menu_state += 1;
        if (local_menu_state == 5)
        {
            local_menu_state = 1;
        }
    }
    
    // Operate in menu
    switch(local_menu_state)
    {
        case 0:
            break;
        case 1: // Azimuth Motor Control
            if (b1) // CCW
            {
                command.command = CMD_AZ_NEG;
            }
            else if (b3) // CW
            {
                command.command = CMD_AZ_POS;
            }
            else // stop motor
            {
                command.command = CMD_AZ_STOP;
            }
            break;
        case 2: // Altitude Motor Control
            if (b1) // Down
            {
                command.command = CMD_ALT_NEG;
            }
            else if (b3) // Up
            {
                command.command = CMD_ALT_POS;
            }
            else // stop motor
            {
                command.command = CMD_ALT_STOP;
            }
            break;
        case 3: // Az - Zero - El ///// This menu will zero az if left button clicked or zero alt(el) if right button clicked
            if (b1) // zero the azimuth
            {
                command.command = CMD_AZ_RESET;
            }
            else if (b3) // zero the altitude
            {
                command.command = CMD_ALT_RESET;
            }
            break;
        case 4: // Quit
            if (b3)
            {
                command.source = CMD_SRC_PC;
                local_menu_state = 0; // reset
            }
            break;
    } 
}

////////////////////////////////////////////////////////////////////////////////
// Get commands from control PC & store as status
void read_pc_commands_temp(void)
{
    char input = get_char_usb(); // get PC input
    
    // Determine if it is local control --- local control has priority and stops PC commands
    if (command.source != CMD_SRC_LOCAL)
    {
        command.source = CMD_SRC_PC;
    
        switch(input)
        {
            case -1: // standard for no input
                break;
            case '1': // stop both motors
                command.command = CMD_STOP; 
                break;
            case '2': // altitude move up
                command.command = CMD_ALT_POS;
                break;
            case '3': // altitude move down
                command.command = CMD_ALT_NEG;
                break;
            case '4': // altitude motor stop
                command.command = CMD_ALT_STOP;
                break;
            case '5': // reset altitude encoders to zero
                command.command = CMD_ALT_RESET;
                break;
            case '6': // azimuth clockwise
                command.command = CMD_AZ_POS;
                break;
            case '7': // azimuth counter-clockwise
                command.command = CMD_AZ_NEG;
                break;
            case '8': // stop azimuth motor
                command.command = CMD_AZ_STOP;
                break;
            case '9': // reset azimuth encoders to zero
                command.command = CMD_AZ_RESET;
                break;
            case '0': // take the motors to home (0,0)
                command.command = CMD_HOME;
                break;
            case ESC:
                print_usb_option_screen();
            // need a case later to restart the program
            default:
                command.command = CMD_STOP;
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// convert rotary encoder pulses to angles
void convert_angles(void)
{
    // Store angles, in tenths of a degree.
    // If we add up the pulses from both encoders and convert from the gear ratio,
    // we get 792.222 pulses per degree output. 
    // 4/19/23 - Changed values to signed to deal with zero crossing - ZM
    motor.az.degrees = (motor.az.pulse1 + motor.az.pulse2)/79.22;
    motor.alt.degrees = (motor.alt.pulse1 + motor.alt.pulse2)/79.22;
}

////////////////////////////////////////////////////////////////////////////////
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
        EL_CONTROL1_SetLow();
        EL_CONTROL2_SetLow();
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
        AZ_CONTROL1_SetLow();
        AZ_CONTROL2_SetLow();
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

////////////////////////////////////////////////////////////////////////////////
// Update the direction of the motors
void update_motors(void)
{
    // Stop Motors
    if (motor.alt.dir == MOTOR_STOP)
    {
        AZ_CONTROL1_SetLow();
        AZ_CONTROL2_SetLow();
        motor.alt.dir = MOTOR_STOP;
    }
    if (motor.az.dir == MOTOR_STOP)
    {
        EL_CONTROL1_SetLow();
        EL_CONTROL2_SetLow();
        motor.az.dir = MOTOR_STOP;
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
    
    prev_alt_dir = motor.alt.dir;
    prev_az_dir = motor.az.dir;
}
