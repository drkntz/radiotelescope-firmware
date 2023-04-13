/* common.c
 * Common libraries and constants for radio-telescope control system project
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/22/2023
*/

#include "common.h"

// These are the motor directions
typedef enum 
{
    MOTOR_STOP, // 0
    MOTOR_POS,  // 1
    MOTOR_NEG   // 2
} motor_dir_t;

// Generic motor states
// altitude is between 0 (horizon) and 90 (zenith) typically
// degrees are sent to the main computer in tenths of a degree
// IE 0-3600 = 0.0-360.0 degrees
// TODO: should we track a resolution greater than that? I mean, the pulses are much higher resolution than the backlash spec
struct Motor
{
    uint16_t degrees;
    motor_dir_t movement;
    uint16_t current; // TODO: how are we formatting current?
    //     uint16_t pulses; TODO: how to store pulses?
    // the pulses are ~35650 per 90 degrees so a full rotation would be 142,600. which wouldn't fit in a uint16
    // and we also have two pulse encoders in quadrature so how should we store that information?
};

// Store motor movement & position state
struct motor_state{
    struct Motor alt;
    struct Motor az;
};

// wait until we get a character from tag connect
char get_char_wait_tag(void)
{
    while(!UART2_IsRxReady())
        ClrWdt();
    
    return UART2_Read();
}

// do not wait, get char if available
char get_char_tag(void)
{
    if(UART2_IsRxReady())
        return UART2_Read();    
    return -1; // TODO: i think this is what to return. EOF?
}