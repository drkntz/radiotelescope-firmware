/*==============================================================================
 * File: pin_manager.h
 * MCC-generated (and human modified) file containing hardware-specific 
 * pin information
 * 
 * Written by MCC and Zach Martin
 * 
 * Radiotelescope project
*/

#ifndef _PIN_MANAGER_H
#define _PIN_MANAGER_H

#include <xc.h>

/* The following section is an abridged version of the MCC-generated pin
 * interface. Each pin had a SetHigh(), SetLow(), Toggle(), GetValue(),
 * SetDigitalInput(), and SetDigitalOutput() macro and each macro had a 
 * corresponding dozen or so lines of redundant comments instructing on its 
 * use. This has been shortened, which saves over 4,500 lines in the file. */
 
#define AZ_CONTROL1_SetHigh()          (_LATA0 = 1)
#define AZ_CONTROL1_SetLow()           (_LATA0 = 0)
#define AZ_CONTROL1_Toggle()           (_LATA0 ^= 1)
#define AZ_CONTROL1_GetValue()         _RA0
#define AZ_CONTROL1_SetDigitalInput()  (_TRISA0 = 1)
#define AZ_CONTROL1_SetDigitalOutput() (_TRISA0 = 0)
#define AZ_CONTROL2_SetHigh()          (_LATA1 = 1)
#define AZ_CONTROL2_SetLow()           (_LATA1 = 0)
#define AZ_CONTROL2_Toggle()           (_LATA1 ^= 1)
#define AZ_CONTROL2_GetValue()         _RA1
#define AZ_CONTROL2_SetDigitalInput()  (_TRISA1 = 1)
#define AZ_CONTROL2_SetDigitalOutput() (_TRISA1 = 0)
#define TP3_SetHigh()          (_LATA10 = 1)
#define TP3_SetLow()           (_LATA10 = 0)
#define TP3_Toggle()           (_LATA10 ^= 1)
#define TP3_GetValue()         _RA10
#define TP3_SetDigitalInput()  (_TRISA10 = 1)
#define TP3_SetDigitalOutput() (_TRISA10 = 0)
#define TP4_SetHigh()          (_LATA11 = 1)
#define TP4_SetLow()           (_LATA11 = 0)
#define TP4_Toggle()           (_LATA11 ^= 1)
#define TP4_GetValue()         _RA11
#define TP4_SetDigitalInput()  (_TRISA11 = 1)
#define TP4_SetDigitalOutput() (_TRISA11 = 0)
#define EL_CONTROL1_SetHigh()          (_LATA2 = 1)
#define EL_CONTROL1_SetLow()           (_LATA2 = 0)
#define EL_CONTROL1_Toggle()           (_LATA2 ^= 1)
#define EL_CONTROL1_GetValue()         _RA2
#define EL_CONTROL1_SetDigitalInput()  (_TRISA2 = 1)
#define EL_CONTROL1_SetDigitalOutput() (_TRISA2 = 0)
#define EL_CONTROL2_SetHigh()          (_LATA3 = 1)
#define EL_CONTROL2_SetLow()           (_LATA3 = 0)
#define EL_CONTROL2_Toggle()           (_LATA3 ^= 1)
#define EL_CONTROL2_GetValue()         _RA3
#define EL_CONTROL2_SetDigitalInput()  (_TRISA3 = 1)
#define EL_CONTROL2_SetDigitalOutput() (_TRISA3 = 0)
#define AZ_ENCODER1_SetHigh()          (_LATA4 = 1)
#define AZ_ENCODER1_SetLow()           (_LATA4 = 0)
#define AZ_ENCODER1_Toggle()           (_LATA4 ^= 1)
#define AZ_ENCODER1_GetValue()         _RA4
#define AZ_ENCODER1_SetDigitalInput()  (_TRISA4 = 1)
#define AZ_ENCODER1_SetDigitalOutput() (_TRISA4 = 0)
#define AZ_ENCODER2_SetHigh()          (_LATA7 = 1)
#define AZ_ENCODER2_SetLow()           (_LATA7 = 0)
#define AZ_ENCODER2_Toggle()           (_LATA7 ^= 1)
#define AZ_ENCODER2_GetValue()         _RA7
#define AZ_ENCODER2_SetDigitalInput()  (_TRISA7 = 1)
#define AZ_ENCODER2_SetDigitalOutput() (_TRISA7 = 0)
#define EL_ENCODER1_SetHigh()          (_LATA8 = 1)
#define EL_ENCODER1_SetLow()           (_LATA8 = 0)
#define EL_ENCODER1_Toggle()           (_LATA8 ^= 1)
#define EL_ENCODER1_GetValue()         _RA8
#define EL_ENCODER1_SetDigitalInput()  (_TRISA8 = 1)
#define EL_ENCODER1_SetDigitalOutput() (_TRISA8 = 0)
#define EL_ENCODER2_SetHigh()          (_LATA9 = 1)
#define EL_ENCODER2_SetLow()           (_LATA9 = 0)
#define EL_ENCODER2_Toggle()           (_LATA9 ^= 1)
#define EL_ENCODER2_GetValue()         _RA9
#define EL_ENCODER2_SetDigitalInput()  (_TRISA9 = 1)
#define EL_ENCODER2_SetDigitalOutput() (_TRISA9 = 0)
#define AUX1_SetHigh()          (_LATB10 = 1)
#define AUX1_SetLow()           (_LATB10 = 0)
#define AUX1_Toggle()           (_LATB10 ^= 1)
#define AUX1_GetValue()         _RB10
#define AUX1_SetDigitalInput()  (_TRISB10 = 1)
#define AUX1_SetDigitalOutput() (_TRISB10 = 0)
#define AUX2_SetHigh()          (_LATB11 = 1)
#define AUX2_SetLow()           (_LATB11 = 0)
#define AUX2_Toggle()           (_LATB11 ^= 1)
#define AUX2_GetValue()         _RB11
#define AUX2_SetDigitalOutput() (_TRISB11 = 0)
#define SPARE3_SetHigh()          (_LATB12 = 1)
#define SPARE3_SetLow()           (_LATB12 = 0)
#define SPARE3_Toggle()           (_LATB12 ^= 1)
#define SPARE3_GetValue()         _RB12
#define SPARE3_SetDigitalInput()  (_TRISB12 = 1)
#define SPARE3_SetDigitalOutput() (_TRISB12 = 0)
#define SPARE2_SetHigh()          (_LATB13 = 1)
#define SPARE2_SetLow()           (_LATB13 = 0)
#define SPARE2_Toggle()           (_LATB13 ^= 1)
#define SPARE2_GetValue()         _RB13
#define SPARE2_SetDigitalInput()  (_TRISB13 = 1)
#define SPARE2_SetDigitalOutput() (_TRISB13 = 0)
#define TP16_SetHigh()          (_LATB14 = 1)
#define TP16_SetLow()           (_LATB14 = 0)
#define TP16_Toggle()           (_LATB14 ^= 1)
#define TP16_GetValue()         _RB14
#define TP16_SetDigitalInput()  (_TRISB14 = 1)
#define TP16_SetDigitalOutput() (_TRISB14 = 0)
#define TP17_SetHigh()          (_LATB15 = 1)
#define TP17_SetLow()           (_LATB15 = 0)
#define TP17_Toggle()           (_LATB15 ^= 1)
#define TP17_GetValue()         _RB15
#define TP17_SetDigitalInput()  (_TRISB15 = 1)
#define TP17_SetDigitalOutput() (_TRISB15 = 0)
#define SPARE7_SetHigh()          (_LATB4 = 1)
#define SPARE7_SetLow()           (_LATB4 = 0)
#define SPARE7_Toggle()           (_LATB4 ^= 1)
#define SPARE7_GetValue()         _RB4
#define SPARE7_SetDigitalInput()  (_TRISB4 = 1)
#define SPARE7_SetDigitalOutput() (_TRISB4 = 0)
#define SPARE6_SetHigh()          (_LATB5 = 1)
#define SPARE6_SetLow()           (_LATB5 = 0)
#define SPARE6_Toggle()           (_LATB5 ^= 1)
#define SPARE6_GetValue()         _RB5
#define SPARE6_SetDigitalInput()  (_TRISB5 = 1)
#define SPARE6_SetDigitalOutput() (_TRISB5 = 0)
#define SPARE5_SetHigh()          (_LATB6 = 1)
#define SPARE5_SetLow()           (_LATB6 = 0)
#define SPARE5_Toggle()           (_LATB6 ^= 1)
#define SPARE5_GetValue()         _RB6
#define SPARE5_SetDigitalInput()  (_TRISB6 = 1)
#define SPARE5_SetDigitalOutput() (_TRISB6 = 0)
#define SPARE4_SetHigh()          (_LATB7 = 1)
#define SPARE4_SetLow()           (_LATB7 = 0)
#define SPARE4_Toggle()           (_LATB7 ^= 1)
#define SPARE4_GetValue()         _RB7
#define SPARE4_SetDigitalInput()  (_TRISB7 = 1)
#define SPARE4_SetDigitalOutput() (_TRISB7 = 0)
#define nSTAT_LED_SetHigh()          (_LATC0 = 1)
#define nSTAT_LED_SetLow()           (_LATC0 = 0)
#define nSTAT_LED_Toggle()           (_LATC0 ^= 1)
#define nSTAT_LED_GetValue()         _RC0
#define nSTAT_LED_SetDigitalInput()  (_TRISC0 = 1)
#define nSTAT_LED_SetDigitalOutput() (_TRISC0 = 0)
#define EXT_LED_SetHigh()          (_LATC1 = 1)
#define EXT_LED_SetLow()           (_LATC1 = 0)
#define EXT_LED_Toggle()           (_LATC1 ^= 1)
#define EXT_LED_GetValue()         _RC1
#define EXT_LED_SetDigitalInput()  (_TRISC1 = 1)
#define EXT_LED_SetDigitalOutput() (_TRISC1 = 0)
#define BUTTON1_SetHigh()          (_LATC2 = 1)
#define BUTTON1_SetLow()           (_LATC2 = 0)
#define BUTTON1_Toggle()           (_LATC2 ^= 1)
#define BUTTON1_GetValue()         _RC2
#define BUTTON1_SetDigitalInput()  (_TRISC2 = 1)
#define BUTTON1_SetDigitalOutput() (_TRISC2 = 0)
#define BUTTON2_SetHigh()          (_LATC3 = 1)
#define BUTTON2_SetLow()           (_LATC3 = 0)
#define BUTTON2_Toggle()           (_LATC3 ^= 1)
#define BUTTON2_GetValue()         _RC3
#define BUTTON2_SetDigitalInput()  (_TRISC3 = 1)
#define BUTTON2_SetDigitalOutput() (_TRISC3 = 0)
#define BUTTON3_SetHigh()          (_LATC4 = 1)
#define BUTTON3_SetLow()           (_LATC4 = 0)
#define BUTTON3_Toggle()           (_LATC4 ^= 1)
#define BUTTON3_GetValue()         _RC4
#define BUTTON3_SetDigitalInput()  (_TRISC4 = 1)
#define BUTTON3_SetDigitalOutput() (_TRISC4 = 0)
#define DIAGS_SetHigh()          (_LATC5 = 1)
#define DIAGS_SetLow()           (_LATC5 = 0)
#define DIAGS_Toggle()           (_LATC5 ^= 1)
#define DIAGS_GetValue()         _RC5
#define DIAGS_SetDigitalInput()  (_TRISC5 = 1)
#define DIAGS_SetDigitalOutput() (_TRISC5 = 0)
#define TP6_SetHigh()          (_LATC8 = 1)
#define TP6_SetLow()           (_LATC8 = 0)
#define TP6_Toggle()           (_LATC8 ^= 1)
#define TP6_GetValue()         _RC8
#define TP6_SetDigitalInput()  (_TRISC8 = 1)
#define TP6_SetDigitalOutput() (_TRISC8 = 0)
#define TP7_SetHigh()          (_LATC9 = 1)
#define TP7_SetLow()           (_LATC9 = 0)
#define TP7_Toggle()           (_LATC9 ^= 1)
#define TP7_GetValue()         _RC9
#define TP7_SetDigitalInput()  (_TRISC9 = 1)
#define TP7_SetDigitalOutput() (_TRISC9 = 0)

// Function prototypes
void PIN_MANAGER_Initialize (void);
void CN_CallBack(void);
void CN_SetInterruptHandler(void (* InterruptHandler)(void));

void __attribute__((deprecated("\nThis will be removed in future MCC releases. \nUse CN_SetInterruptHandler instead."))) CN_SetIOCInterruptHandler(void *handler);

#endif
