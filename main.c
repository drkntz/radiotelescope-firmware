/* main.c
 * Main file for radio-telescope control system project
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/9/2023
 * 
 * TODO: 
 *  * Setup TMR1 as default time base & setup interval checks
 *  * diagnostics via diags button and some kind of passcode ('q' 3 times over usb?)
*/


#include <xc.h>
#include "common.h"
#include "mcc_generated_files/uart2.h"
#include "diagnostic.h"
#include "mcc_generated_files/uart1.h"

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    //nSTAT_LED_SetDigitalOutput(); // i think this is already done in mcc? TODO
    
    print_uartx = PRINT_TAG;
    printf("\r\nWelcome to the Radio Telescope Control system \r\n");
    print_uartx = PRINT_BOTH; // for now, both UARTs will work over printf
    
    diagnostic_main(); // check pins
    
    char test = 0;
    
    while (1)
    {           
        //TMR1_Tasks_16BitOperation();

        test = get_char_tag();
        if(test != -1) printf("\r\n test = %c", test);
        nSTAT_LED_Toggle();
        ClrWdt();
        __delay_ms(500);
    }

    return 1;
}

