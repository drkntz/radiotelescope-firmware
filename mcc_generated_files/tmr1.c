/*==============================================================================
 * File: tmr1.c
 * MCC-generated (and human modified) timer interface for TMR1 peripheral.
 * Radiotelescope project
*/

#include <stdio.h>
#include "tmr1.h"

typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed */
    volatile bool           timerElapsed;
    /*Software Counter value*/
    volatile uint8_t        count;

} TMR_OBJ;

static TMR_OBJ tmr1_obj;

/**
  Section: Driver Interface
*/

void TMR1_Initialize (void)
{
    //TMR1 0; 
    TMR1 = 0x00;
    //Period = 0.001 s; Frequency = 2000 Hz; PR1 1; 
    PR1 = 0x01;
    //TCKPS 1:1; TON enabled; TSIDL disabled; TCS External; TECS T1CK; TSYNC disabled; TGATE disabled; 
    T1CON = 0x8102;
    T1CONbits.T1ECS = 0b10; // Source = LPRC (31kHz)
    T1CONbits.TCKPS = 0b01; // 1:8 prescaler, so we have 3.875kHz now
	
    tmr1_obj.timerElapsed = false;

}


void TMR1_Tasks_16BitOperation( void )
{
    /* Check if the Timer Interrupt/Status is set */
    if(IFS0bits.T1IF)
    {
        tmr1_obj.count++;
        tmr1_obj.timerElapsed = true;
        IFS0bits.T1IF = false;
    }
}

void TMR1_Period16BitSet( uint16_t value )
{
    /* Update the counter values */
    PR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Period16BitGet( void )
{
    return( PR1 );
}

void TMR1_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values */
    TMR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Counter16BitGet( void )
{
    return( TMR1 );
}




void TMR1_Start( void )
{
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;


    /* Start the Timer */
    T1CONbits.TON = 1;
}

void TMR1_Stop( void )
{
    /* Stop the Timer */
    T1CONbits.TON = false;

}

bool TMR1_GetElapsedThenClear(void)
{
    bool status;
    
    status = tmr1_obj.timerElapsed;

    if(status == true)
    {
        tmr1_obj.timerElapsed = false;
    }
    return status;
}

int TMR1_SoftwareCounterGet(void)
{
    return tmr1_obj.count;
}

void TMR1_SoftwareCounterClear(void)
{
    tmr1_obj.count = 0; 
}

/* Convert timer1 ticks to ms
 * This method has a max time of 65,536 counts. 
 * Period is 1/3.875k = 258.06us. Max time ~= 16.9 seconds.
 */
uint16_t timestamp_to_ms(uint16_t timestamp)
{
    uint16_t result;
    result = timestamp/3.875;
    return result; 
}
        
/* get "raw" timestamp
 * this is the clock ticks of TMR1
 */
uint16_t timestamp_raw(void)
{
    return TMR1_Counter16BitGet();
}