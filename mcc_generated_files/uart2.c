/*==============================================================================
 * File: uart2.c
 * MCC-generated (and human modified) driver for UART2 peripheral.
 * Radiotelescope project
*/

#include <xc.h>
#include "uart2.h"
#include "uart1.h"
#include "../LiquidCrystal_pic.h"

// Used this as a cheap way to pass a handle to the printf function
// by setting the global print_output before using printf.
uint8_t print_output;

void UART2_Initialize(void)
{
/**    
     Set the UART2 module to the options selected in the user interface.
     Make sure to set LAT bit corresponding to TxPin as high before UART initialization
*/
    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U2MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U2STA = 0x00;
    // BaudRate = 9600; Frequency = 16000000 Hz; BRG 416; 
    U2BRG = 0x1A0;
    
    U2MODEbits.UARTEN = 1;   // enabling UART ON bit
    U2STAbits.UTXEN = 1;
}

uint8_t UART2_Read(void)
{
    while(!(U2STAbits.URXDA == 1))
    {
        
    }

    if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
    }
    
    return U2RXREG;
}

void UART2_Write(uint8_t txData)
{
    while(U2STAbits.UTXBF == 1)
    {
        
    }

    U2TXREG = txData;    // Write the data byte to the USART.
}

bool UART2_IsRxReady(void)
{
    return U2STAbits.URXDA;
}

bool UART2_IsTxReady(void)
{
    return ((!U2STAbits.UTXBF) && U2STAbits.UTXEN );
}

bool UART2_IsTxDone(void)
{
    return U2STAbits.TRMT;
}

int __attribute__((__section__(".libc.write"))) write(int handle, void *buffer, unsigned int len) 
{
    unsigned int i;

    for (i = len; i; --i)
    {
        switch(print_output)
        {
            case PRINT_USB: // UART1, USB connection
                UART1_Write(*(char*)buffer++);
                break;
                
            case PRINT_BOTH: // print both of them
                UART1_Write(*(char*)buffer);
                UART2_Write(*(char*)buffer++);
                break;
                
            case PRINT_TAG:
                UART2_Write(*(char*)buffer++);
                break;
                
            case PRINT_LCD:
                lcd_printf_write(*(char*)buffer++);
                break;
                
            default: // debug, UART2
                UART2_Write(*(char*)buffer++);
                break;
        } 
    }
    return(len);
}