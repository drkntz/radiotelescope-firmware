/* File: protocol.c
 * Handle communication protocol with main PC over USB-UART bridge.
 * Authors: Zach Martin & Aaron Olsen
 * Date: 4/16/23
 * 
*/

#include "protocol.h"
#include "common.h"

#define MAX_FRAME_SIZE 7    // Max 7 bytes: CMD CMD ALT ALT AZ AZ EOT
#define EOT 0x04            // ASCII EOT character

/* Check for incoming commands over USB-UART.
 * Rough outline of protocol is given in:
 * https://github.com/RidhaChowdhury/MiniRadioTelescope/blob/master/ControlBoard/protocol_description.txt
 * 
 * Returns which command was rx'd
 */
commands_t check_pc_commands(void)
{
    static uint8_t bytenum = 0; // keep track of number of rx'd bytes
    static uint8_t state; 
    
    static char frame [MAX_FRAME_SIZE] = {0x00};
    
    // Put the command into the frame buffer
    while(UART1_IsRxReady())
    {
        frame[bytenum] = UART1_Read(); // Read one byte
        if(frame[bytenum] == EOT)
        {
            bytenum = 0; // reset frame
        }
        bytenum ++; // move to next byte
            
    }
    return CMD_NONE; // no data was rx'd
}