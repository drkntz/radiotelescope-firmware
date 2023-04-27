/*==============================================================================
 * File: pin_manager.c
 * MCC-generated (and human modified) file containing hardware-specific 
 * pin information and interrupt on change
 * 
 * Written by MCC and Zach Martin
 * 
 * Radiotelescope project
*/

#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"
#include "../common.h"

void (*CN_InterruptHandler)(void) = NULL;

void PIN_MANAGER_Initialize (void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0001;
    LATC = 0x0081;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x0F90;
    TRISB = 0xFFFF;
    TRISC = 0x037C;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPD1 = 0x0000;
    CNPD2 = 0x0000;
    CNPD3 = 0x0000;
    CNPU1 = 0x0000;
    CNPU2 = 0x0000;
    CNPU3 = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;
    ODCC = 0x0001;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSA = 0x0000;
    ANSB = 0x000C;
    ANSC = 0x0000;
    
    /****************************************************************************
     * Interrupt On Change: any
     ***************************************************************************/
    CNEN1bits.CN0IE = 1;    //Pin : RA4
    CNEN1bits.CN9IE = 1;    //Pin : RA7
    CNEN3bits.CN33IE = 1;    //Pin : RA8
    CNEN3bits.CN34IE = 1;    //Pin : RA9
    
    /* Initialize IOC Interrupt Handler*/
    CN_SetInterruptHandler(&CN_CallBack);
    
    /****************************************************************************
     * Interrupt On Change: Interrupt Enable
     ***************************************************************************/
    IFS1bits.CNIF = 0; //Clear CNI interrupt flag
    IEC1bits.CNIE = 1; //Enable CNI interrupt
}

// Local variables to keep track of change in encoder pin state
static bool az1_encoder_state = false;
static bool az2_encoder_state = false;
static bool el1_encoder_state = false;
static bool el2_encoder_state = false;

// This is the default callback for pin changes.
// This runs when the encoder has changed
void  CN_CallBack(void)
{
    static motor_dir_t az_dir, el_dir;
    
    // Retain old state during a stop that way we can account for drifting/overshoot
    if(motor.az.dir != MOTOR_STOP)
    {
        az_dir = motor.az.dir;
    }
    if(motor.alt.dir != MOTOR_STOP)
    {
        el_dir = motor.alt.dir;
    }
    
    // Check which pin has changed state and increment the pulse counter
    bool temp = AZ_ENCODER1_GetValue(); 
    
    if(az1_encoder_state != temp)
    {
        az1_encoder_state = temp;
        if(az_dir == MOTOR_NEG)
        {
            motor.az.pulse1 --;
        }
        else
        {
            motor.az.pulse1 ++;
        }
    }
    
    temp = AZ_ENCODER2_GetValue(); 
    
    if(az2_encoder_state != temp)
    {
        az2_encoder_state = temp;
        if(az_dir == MOTOR_NEG)
        {
            motor.az.pulse2 --;
        }
        else
        {
            motor.az.pulse2 ++;
        }
    }
    
    temp = EL_ENCODER1_GetValue(); 
    
    if(el1_encoder_state != temp)
    {
        el1_encoder_state = temp;
        if(el_dir == MOTOR_NEG)
        {
            motor.alt.pulse1 --;
        }
        else
        {
            motor.alt.pulse1 ++;
        }
    }
    
    temp = EL_ENCODER2_GetValue(); 
    
    if(el2_encoder_state != temp)
    {
        el2_encoder_state = temp;
        if(el_dir == MOTOR_NEG)
        {
            motor.alt.pulse2 --;
        }
        else
        {
            motor.alt.pulse2 ++;
        }
    }
}

void CN_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC1bits.CNIE = 0; //Disable CNI interrupt
    CN_InterruptHandler = InterruptHandler; 
    IEC1bits.CNIE = 1; //Enable CNI interrupt
}

void CN_SetIOCInterruptHandler(void *handler)
{ 
    CN_SetInterruptHandler(handler);
}

/* Interrupt service routine for the CNI interrupt. */
void __attribute__ (( interrupt, no_auto_psv )) _CNInterrupt ( void )
{
    if(IFS1bits.CNIF == 1)
    {
        if(CN_InterruptHandler) 
        { 
            CN_InterruptHandler(); 
        }
        
        // Clear the flag
        IFS1bits.CNIF = 0;
    }
}

