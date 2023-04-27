/*==============================================================================
 * File: tmr1.h
 * MCC-generated (and human modified) timer interface for TMR1 peripheral.
 * Radiotelescope project
*/

#ifndef _TMR1_H
#define _TMR1_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

void TMR1_Initialize (void);

void TMR1_Tasks_16BitOperation( void );

void TMR1_Period16BitSet( uint16_t value );

uint16_t TMR1_Period16BitGet( void );

void TMR1_Counter16BitSet ( uint16_t value );

uint16_t TMR1_Counter16BitGet( void );

void TMR1_Start( void );

void TMR1_Stop( void );

bool TMR1_GetElapsedThenClear(void);

int TMR1_SoftwareCounterGet(void);

void TMR1_SoftwareCounterClear(void);

// get raw ticks. This is the same as timer1 counter 16bit get
uint16_t timestamp_raw(void);

// convert raw timestamp to ms
uint16_t timestamp_to_ms(uint16_t timestamp);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif //_TMR1_H