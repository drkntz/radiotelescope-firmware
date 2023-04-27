/*==============================================================================
 * File: uart1.h
 * MCC-generated (and human modified) driver for UART1 peripheral.
 * Radiotelescope project
*/


#ifndef _UART1_H
#define _UART1_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

void UART1_Initialize(void);

uint8_t UART1_Read(void);
void UART1_Write(uint8_t txData);
bool UART1_IsRxReady(void);
bool UART1_IsTxReady(void);

bool UART1_IsTxDone(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif  // _UART1_H