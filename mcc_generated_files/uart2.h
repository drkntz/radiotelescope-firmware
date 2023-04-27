/*==============================================================================
 * File: uart2.h
 * MCC-generated (and human modified) driver for UART2 peripheral.
 * Radiotelescope project
*/

#ifndef _UART2_H
#define _UART2_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

enum {
    PRINT_TAG, // uart2
    PRINT_USB, // uart1
    PRINT_BOTH, // both UARTs
    PRINT_LCD  // print only on LCD
};

extern uint8_t print_output; // which one to print to 

void UART2_Initialize(void);
uint8_t UART2_Read(void);
void UART2_Write(uint8_t txData);
bool UART2_IsRxReady(void);
bool UART2_IsTxReady(void);
bool UART2_IsTxDone(void);
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif  // _UART2_H