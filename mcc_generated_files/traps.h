/*==============================================================================
 * File: traps.h
 * MCC-generated system traps file
 * Radiotelescope project
*/

#ifndef _TRAPS_H
#define _TRAPS_H

#include <stdint.h>

/**
 * Error codes
 */
typedef enum 
{
    /* ----- Traps ----- */
    TRAPS_OSC_FAIL = 0, /** Oscillator Fail Trap vector */
    TRAPS_STACK_ERR = 1, /** Stack Error Trap vector */
    TRAPS_ADDRESS_ERR = 2, /** Address error Trap vector */
    TRAPS_MATH_ERR = 3, /** Math Error Trap vector */
} TRAPS_ERROR_CODE;

// Default handler for the traps. Stores trap error code and waits forever if
// a trap happens.
void TRAPS_halt_on_error(uint16_t code);

#endif