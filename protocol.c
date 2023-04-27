/*==============================================================================
 * File: protocol.c
 * Handle communication protocol with main PC over USB-UART bridge.
 * Authors: Zach Martin & Aaron Olsen
 * Date: 4/16/23
	* TODO: handle returning status
*/

#include "protocol.h"
#include "common.h"

#define MAX_FRAME_SIZE 7    // Max 7 bytes: CMD CMD ALT ALT AZ AZ EOT
#define EOT 0x04            // ASCII EOT character

// Protocol states
typedef enum {
    PC_WAITING, // waiting for valid command
    PC_ALT_H,   // valid command rx'd, now getting alt degree high byte
    PC_ALT_L,
    PC_AZ_H,
    PC_AZ_L,
    PC_EOT      // now waiting for EOT
} pc_state_t;

////////////////////////////////////////////////////////////////////////////////
// Function prototypes
commands_t _is_command(char hibyte, char lowbyte);
uint16_t _bytes_to_uint16(uint8_t hibyte, uint8_t lowbyte);

/* Check for incoming commands over USB-UART.
 * Rough outline of protocol is given in:
 * https://github.com/RidhaChowdhury/MiniRadioTelescope/blob/master/ControlBoard/protocol_description.txt
 * 
 * Returns which command was rx'd
 */
commands_t check_pc_commands(void)
{
    static char prev_byte, new_byte;
    static uint16_t new_alt_deg, new_az_deg;
    static commands_t new_command; 
    static pc_state_t state = PC_WAITING; 
    commands_t junk_command;
    
    // Get new characters, if available
    while(UART1_IsRxReady())
    {
        prev_byte = new_byte;       // Both degrees and commands are given in two-byte values.
        new_byte = UART1_Read();    // Read newest byte.
        
        switch(state)
        {
            case PC_WAITING:
                junk_command = _is_command(prev_byte, new_byte);
                if(junk_command != CMD_NONE) // valid command rx'd!
                {
                    new_command = junk_command;
                    state = PC_ALT_H;
                }
                break;
            case PC_ALT_H:
                state = PC_ALT_L;
                break;
            case PC_ALT_L:
                new_alt_deg = _bytes_to_uint16(prev_byte, new_byte);
                state = PC_AZ_H;
                break;
            case PC_AZ_H:
                state = PC_AZ_L;
                break;
            case PC_AZ_L:
                new_az_deg = _bytes_to_uint16(prev_byte, new_byte);
                state = PC_EOT;
                break;
            case PC_EOT:
                if(new_byte == EOT)
                {
                    command.command = new_command;
                    command.alt_deg = new_alt_deg;
                    command.az_deg = new_az_deg;
                    command.source = CMD_SRC_PC;
                    state = PC_WAITING;
                    return new_command; // DONE!!!
                }
                else // assume that we got some data corruption, for now just throw out the whole frame. TODO
                {
                    state = PC_WAITING;
                }
                break;
        }            
    }    
    return CMD_NONE; // no data was rx'd
}

// Check if the byte inputs are commands per Mark P's command structure.
// Return CMD type (EG CMD_GOTO)
commands_t _is_command(char hibyte, char lowbyte)
{
    switch(hibyte)
    {
        case 'c':
            switch(lowbyte)
            {
                case 'l': return CMD_ALT_RESET;
                case 'z': return CMD_AZ_RESET;
                case 't': return CMD_NONE; // TODO!!! Set twist??? talk to Mark
            }
        case 'm':
            switch(lowbyte)
            {
                case 'l': return CMD_ALT_POS;
                case 'z': return CMD_AZ_POS;
            }
        case 'w':
            switch(lowbyte)
            {
                case 'l': return CMD_ALT_NEG;
                case 'z': return CMD_AZ_NEG;
            }
        case 'g':
            if(lowbyte == ' ') return CMD_GOTO;
        case 'x':
            switch(lowbyte)
            {
                case 'l': return CMD_ALT_STOP;
                case 'z': return CMD_AZ_STOP;
                case 'a': return CMD_STOP;
            }
        case 'r':
            if(lowbyte == ' ') return CMD_REPORT_STATUS; 
    }

    return CMD_NONE; // Assume invalid command
}

// convert two bytes to uint16_t
uint16_t _bytes_to_uint16(uint8_t hibyte, uint8_t lowbyte)
{
    uint16_t res;
    res = hibyte << 8 | lowbyte;
    return res;
}