/*==============================================================================
 * File: common.c
 * Common libraries and constants for radio-telescope control system project
 * Authors: Zach Martin & Aaron Olsen
 * Date: 3/22/2023
*/

#include "common.h"

////////////////////////////////////////////////////////////////////////////////
// State Variables for Motors and Command input

struct _Motors motor;

struct _Command command; // used to keep track of incoming commands

uint8_t local_menu_state = 0;


// wait until we get a character from tag connect
char get_char_wait_tag(void)
{
    while(!UART2_IsRxReady())
    {
        ClrWdt();
    }
    return UART2_Read();
}

// do not wait, get char if available
char get_char_tag(void)
{
    if(UART2_IsRxReady())
        return UART2_Read();    
    return -1;
}

// wait until we get a character from usb-uart
char get_char_wait_usb(void)
{
    while(!UART1_IsRxReady())
    {
        ClrWdt();
    }
    return UART1_Read();
}

// do not wait, get char if available
char get_char_usb(void)
{
    if(UART1_IsRxReady())
        return UART1_Read();    
    return -1;
}

// delay function that accepts variables (unlike builtin macro)
// and resets the watchdog timer
void delay_ms(uint16_t ms)
{
    while(ms--)
    {
        __delay_ms(1);
        ClrWdt();
    }
}

// Convert ascii character to number
// otherwise return 0xFF
uint8_t ascii_to_num(char c)
{
    if(c > 47 && c < 58) // is an ascii number
    {
        c = c - 48; // bit-zap it to the range 0-9
        return (uint8_t)c;
    }
    return 0xFF;
}