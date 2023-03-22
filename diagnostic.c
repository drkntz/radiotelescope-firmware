/* diagnostic.c
 * Test functions for radio-telescope board
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/22/2023
 * 
 * 
*/
#include "diagnostic.h"
#include "common.h"

#define DELAY 500
// flip digital pins for now
void diagnostic_main(void)
{
    printf("\r\nDiagnostic menu. Hit any key to cycle through pin testing");
    get_char_wait_tag();
    
    AZ_CONTROL1_SetHigh(); 
    printf("\r\nAZ Control1 high");
    get_char_wait_tag();

    AZ_CONTROL1_SetLow(); 
    printf("\r\nAZ Control1 low");
    get_char_wait_tag();
    
    AZ_CONTROL2_SetHigh(); 
    printf("\r\nAZ Control 2 high");
    get_char_wait_tag();
    
    AZ_CONTROL2_SetLow(); 
    printf("\r\nAZ Control 2 low");
    get_char_wait_tag();
    
    EL_CONTROL1_SetHigh(); 
    printf("\r\nEL Control1 high");
    get_char_wait_tag();
    EL_CONTROL1_SetLow(); 
    printf("\r\nEL Control1 low");
    get_char_wait_tag();
    
    EL_CONTROL2_SetHigh(); 
    printf("\r\nEL Control 2 high");
    get_char_wait_tag();
    EL_CONTROL2_SetLow(); 
    printf("\r\nEL Control 2 low");
    get_char_wait_tag();
}