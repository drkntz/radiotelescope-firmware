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
#include "mcc_generated_files/clock.h"  // for delay functions
#include "p24FV32KA304.h"               // not sure if this has to be explicitly included? TODO
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "LiquidCrystal_pic.h"
#include "mcc_generated_files/tmr1.h"

// TODO: put program constants here
#define ESC 27 // this is the escape key in ascii
char get_char_wait_tag(void);
char get_char_tag(void);
void delay_ms(uint16_t ms);

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
    uint16_t degrees;   // This is converted from pulses. In tenths of a degree.
    motor_dir_t dir;    // MOTOR_POS, MOTOR_NEG, or MOTOR_STOP
    uint16_t current;   // TODO: how are we formatting current?
    uint32_t pulse1;    // Encoder 1 transition count.
    uint32_t pulse2;    // Encoder 2 transition count.
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
    uint16_t alt_deg;   // used for GOTO and the like
    uint16_t az_deg;    // used for GOTO and the like
};

extern struct _Command command; // used to keep track of incoming commands

#endif	// COMMON_H

