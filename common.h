/* common.h
 * Common libraries and constants for radio-telescope control system project
 * Authors: Zach Martin & Aaron Olsen
 * Date: 3/9/2023
*/

#ifndef COMMON_H
#define	COMMON_H

// FCY is used for delays. Make sure its defined here before libpic30.h is included
#ifndef FCY 
#define FCY (_XTAL_FREQ / 2)
#endif 

#include <xc.h> // include processor files - each processor file is guarded.
#include <libpic30.h>                   // also for delays and other stuff
#include <stdio.h>                      // for printf
#include <stdlib.h>
#include "mcc_generated_files/clock.h"  // for delay functions
#include "p24FV32KA304.h"               // not sure if this has to be explicitly included? TODO
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "LiquidCrystal_pic.h"
#include "mcc_generated_files/tmr1.h"
#include "motor.h"

////////////////////////////////////////////////////////////////////////////////
// Disable some things for capstone presentation TODO: remove
#define PRESENTATION_MODE   // comment or remove this for regular mode


// TODO: put program constants here
#define ESC 27 // this is the escape key in ascii
#define ENTER 10 // this is the enter key in ascii

char get_char_wait_tag(void);
char get_char_tag(void);
char get_char_usb(void);
char get_char_wait_usb(void);
void delay_ms(uint16_t ms);
uint8_t ascii_to_num(char c);

////////////////////////////////////////////////////////////////////////////////
// Motor states

// These are the motor directions
typedef enum 
{
    MOTOR_STOP, // 0
    MOTOR_POS,  // 1
    MOTOR_NEG   // 2
} motor_dir_t;

// Generic motor states
// Altitude is between 0 (horizon) and 90 (zenith) typically.
// Degrees are sent to the main computer in tenths of a degree.
// IE 0-3600 = 0.0-360.0 degrees.
struct _Motor
{
    int16_t degrees;   // This is converted from pulses. In tenths of a degree.
    int16_t max_degrees;    // these are the limits for motion
    int16_t min_degrees;
    motor_dir_t dir;    // MOTOR_POS, MOTOR_NEG, or MOTOR_STOP
    uint16_t current;   // TODO: how are we formatting current?
    int32_t pulse1;    // Encoder 1 transition count.
    int32_t pulse2;    // Encoder 2 transition count.
    uint16_t off_timestamp; // keep track of how long it's been off. Used only in motor movement TODO: make private
    uint16_t pwm_timestamp;
    uint8_t pwm;
    uint8_t speed;
    uint8_t overshoot; // calibration value for overshoot
};

// Motor structure with an entry for each axis.
// Access members of the structure. For example:
// motor.alt.degrees = (motor.alt.pulse1 + motor.alt.pulse2)/79.222;
struct _Motors{
    struct _Motor alt;
    struct _Motor az;
};

extern struct _Motors motor; // Defined in common.c

////////////////////////////////////////////////////////////////////////////////
// Command state variables, to keep track of input commands

// List of valid commands
typedef enum 
{
    CMD_STOP,
    CMD_GOTO,   // GO to position  
            
    CMD_ALT_POS,    // move in + degrees
    CMD_ALT_NEG,    // move in - degrees
    CMD_ALT_STOP,   // stop movement
    CMD_ALT_RESET,  // reset rotary encoders to 0
            
    CMD_AZ_POS,     // move in + degrees
    CMD_AZ_NEG,     // move az - degrees
    CMD_AZ_STOP,    // stop movement
    CMD_AZ_RESET,   // reset rotary encoders to 0

    CMD_REPORT_STATUS,  // Get status of controller
    CMD_HOME,            // GO to 0,0
            
    CMD_NONE
}commands_t;

// List of valid command inputs
typedef enum
{
    CMD_SRC_NONE,   // disconnected
    CMD_SRC_PC,     // From computer over USB
    CMD_SRC_LOCAL,  // From local pushbuttons/LCD
    CMD_SRC_DEBUG   // From debug header
}cmd_src_t;

struct _Command {
    cmd_src_t source;   // which input is the one we're listening to
    commands_t command; // what is the current command we're getting
    int16_t alt_deg;   // used for GOTO and the like
    int16_t az_deg;    // used for GOTO and the like
};

extern struct _Command command; // used to keep track of incoming commands

extern uint8_t local_menu_state;

#include "protocol.h"

#endif	// COMMON_H

