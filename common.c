/* common.c
 * Common libraries and constants for radio-telescope control system project
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/22/2023
*/

#include "common.h"

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