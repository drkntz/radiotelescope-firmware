/**
  PIN MANAGER Generated Driver File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

  @Summary:
    This is the generated manager file for the PIC24 / dsPIC33 / PIC32MM MCUs device.  This manager
    configures the pins direction, initial state, analog setting.

  @Description:
    This source file provides implementations for PIN MANAGER.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.1
        Device            :  PIC24FV32KA304
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB 	          :  MPLAB X v5.50
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/


/**
    Section: Includes
*/

#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"
#include "../common.h"

/**
 Section: File specific functions
*/
void (*CN_InterruptHandler)(void) = NULL;

/**
 Section: Driver Interface Function Definitions
*/
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
// TODO: what happens when encoders drift? we might end up accidentally 
// incrementing the encoder count because default is to increment. 
// TODO: we should stick with one thing, either alt/az or el/az, not swap between
// the two names. 
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

