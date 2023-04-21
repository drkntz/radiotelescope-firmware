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
void update_pc_menu(double, double);
void read_buttons(void);
void _goto(void); // get input from user and send goto command


// Variable declarations for read_buttons()
uint16_t button_time = 0;
uint16_t button_delay = 250;

////////////////////////////////////////////////////////////////////////////////
// Main program. Configure everything and update states in an endless loop.
int main(void)
{
    bool diags;
    double az_deg_print, el_deg_print;
    
    SYSTEM_Initialize();
    
    // TODO: clean up
    motor.alt.speed = 3;
    motor.az.speed = 3;
    motor.alt.pwm = 2;
    motor.az.pwm = 2;
    motor.az.overshoot = 0; // 1/10th of a degree
    motor.alt.overshoot = 1; // very minimal overshoot
    
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
         * read buttons                         done
         * update rotary encoders               done
         * process commands                     mostly done
         * update motors                        done
         * send status at 3Hz (check notes)
         * refresh LCD                          done
         * blink status LEDs                    done
         */

    
        // This makes it easier to print floating point
        az_deg_print = motor.az.degrees/10.0;
        el_deg_print = motor.alt.degrees/10.0;
        update_pc_menu(az_deg_print, el_deg_print);    // Get commands and print menu
        
        read_buttons();      // Read buttons
       
        process_command();   // Update non-movement based commands
        
        convert_angles();   // Convert rotary encoder pulses to angles
        
        update_motors();    // Update motors and handle movement-based commands
        
        refresh_lcd(az_deg_print, el_deg_print); // Refresh LCD at 2Hz
        
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

////////////////////////////////////////////////////////////////////////////////
// Startup message and logo
void welcome_message(void)
{
    print_output = PRINT_USB;
    printf(
"\r\n\n\n        *    *\r\n"
"           *                                              ***\r\n"
"                                                         *****\r\n"
"                                                        ******\r\n"
"                        ***                           *** ****\r\n"
"                        *   **************************** *****\r\n"
"                        **** **                    ***  *****\r\n"
"                         *** **                  **** ******\r\n"
"                          *   ***               ***  ********\r\n"
"                          **    ***          ****  *********\r\n"
"                          ***    ***        ***  **********\r\n"
"                           ***     ***    ***  ***********\r\n"
"                          * ***         ***  ************\r\n"
"                        **   ***      ***  *************\r\n"
"                      **      ***   ***  ************* ****\r\n"
"                    **         ******   ************  **  *\r\n"
"                   **          **** ************* ** **   *\r\n"
"                  **         **** ************  **  **    *\r\n"
"                 ***      **** *********       ******    **\r\n"
"                  ********* *****              **       ***\r\n"
"                                                ***********\r\n"
"Radio Telescope                                  **\r\n"
"Control System                                **  *****   ******\r\n"
"                                          ***  **   **  **   ***\r\n"
"Zach Martin & Aaron Olsen                 *******     ****      **\r\n");
    
    lcd_setCursor(0,0);
    print_output = PRINT_LCD;
    printf("Radio Telescope");
    lcd_setCursor(0,1);
    printf("Control System");
    print_output = PRINT_USB;
}

////////////////////////////////////////////////////////////////////////////////
// Option screen for demonstration
void print_usb_option_screen(void) // UART1 options screen
{
    uint8_t print_op_save = print_output;   // Store current state of printf.
    print_output = PRINT_USB;
    printf("\r\n===================================================================================================="
            "\r\nMenu Options:");
    printf( "\r\n1 - Altitude +"
            "\r\n2 - Altitude -"
            "\r\n3 - Altitude Stop"
            
            "\r\n\n4 - Azimuth +"
            "\r\n5 - Azimuth -"
            "\r\n6 - Azimuth Stop"
            
            "\r\n\n7 - Enter degrees"
            "\r\n\nA - Advanced Menu"
            
            "\r\nSpacebar - stop all motors"
            "\r\n===================================================================================================="
            "\r\n Direction   |     Angle    |  Command"
            "\r\n Alt    Az   |   Alt    Az  |  Source\r\n");
    print_output = print_op_save; // Restore state of printf
}

////////////////////////////////////////////////////////////////////////////////
// Read buttons from enclosure
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
        return;
    }
    
    if(b1 && b3)
    {
        command.command = CMD_STOP;
        return;
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
// This is the menu for capstone demonstration

// Keep track of menu
enum
{
    MENU_STATE_MOVE,
    MENU_STATE_GOTO,
    MENU_STATE_ADVANCED
};

// Handle menu
void update_pc_menu(double az_deg_print, double el_deg_print)
{
    static uint8_t menu_state = MENU_STATE_MOVE; // Default to "Move" state
    char input = get_char_usb(); // get PC input


    // Update menu
    if(menu_state == MENU_STATE_MOVE)
    {
        printf("\r  %x     %x    |   %3.1f    %3.1f |  ",motor.alt.dir, motor.az.dir, el_deg_print, az_deg_print);
        switch(command.source)
        {
            case CMD_SRC_LOCAL:
                printf("Local");
                break;
            case CMD_SRC_PC:
                printf("PC   ");
                break;
        }
    }
    
    if(input == -1 || command.source == CMD_SRC_LOCAL)
    {
        return;
    }
    
    switch(menu_state)
    {
        case MENU_STATE_MOVE: // this is the basic menu state
                switch(input)
                {
                    case '1': // altitude move up
                        command.command = CMD_ALT_POS;
                        break;
                    case '2': // altitude move down
                        command.command = CMD_ALT_NEG;
                        break;
                    case '3': // altitude motor stop
                        command.command = CMD_ALT_STOP;
                        break;
                    case '4': // azimuth +
                        command.command = CMD_AZ_POS;
                        break;
                    case '5': // azimuth -
                        command.command = CMD_AZ_NEG;
                        break;
                    case '6': // azimuth stop
                        command.command = CMD_AZ_STOP;
                        break;
                    case '7': // goto
                        printf("\r\n\n");
                        command.command = CMD_STOP;
                        update_motors();
                        menu_state = MENU_STATE_GOTO;
                        break;
                    case 'A': // take the motors to home (0,0)
                        menu_state = MENU_STATE_ADVANCED;
                        break;
                    case ESC:
                        print_usb_option_screen();
                    // need a case later to restart the program
                    default:
                        command.command = CMD_STOP;
                        break;
                }
            break;
            
        case MENU_STATE_GOTO:
            _goto();
            break;
        case MENU_STATE_ADVANCED:
            command.command = CMD_HOME;
            break;
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
/* Process commands. Motor commands are processed in the motor update
 * */
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
    
    
    // CMD_ALT_RESET - reset alt encoders to zero
    else if (command.command == CMD_ALT_RESET)
    {
        motor.alt.pulse1 = 0;
        motor.alt.pulse2 = 0;
    }

    // CMD_AZ_RESET - reset az encoders to zero
    else if (command.command == CMD_AZ_RESET)
    {
        motor.az.pulse1 = 0;
        motor.az.pulse2 = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Helper for pc menu, go to a specific location
void _goto(void)
{
    static uint8_t goto_state =0;
    static uint8_t tens_position = 1;
    static int16_t azdeg = 0;
    static int16_t altdeg = 0;
    
    uint8_t num_in;
    
    char input = get_char_usb();
    if(input == -1)
    {
        return;
    }
    
    if(input != ESC)
    {
        
        num_in = ascii_to_num(input);
        if(num_in > 10) // is not a legit number
        {
            return;
        }
    }
    
    if(input == ESC || tens_position > 100)
    {
        goto_state ++;
        tens_position = 1;
    }
    
    switch(goto_state)
    {
        case 0: // alt
            altdeg += num_in*tens_position;
            tens_position = tens_position*10;
            break;
        case 1:
            azdeg += num_in*tens_position;
            tens_position = tens_position*10;
            break;
        case 3:
            command.alt_deg = altdeg;
            command.az_deg = azdeg;
            command.command = CMD_GOTO;
            azdeg = 0;
            altdeg = 0;
            goto_state = 0;
            tens_position = 1;
            break;
        default:
            return; // TODO
            
    }
    
    printf("\rGOTO:  ALT: %3.1f   AZ: %3.1f", altdeg/10.0, azdeg/10.0);
}