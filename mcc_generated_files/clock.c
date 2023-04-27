/*==============================================================================
 * File: clock.c
 * MCC-generated clock oscillator driver
 * Radiotelescope project
*/

#include <stdint.h>
#include "xc.h"
#include "clock.h"

void CLOCK_Initialize(void)
{
    // RCDIV FRC/1; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x3000;
    // TUN Center frequency; 
    OSCTUN = 0x00;
    // ROEN disabled; ROSEL FOSC; RODIV 0; ROSSLP disabled; 
    REFOCON = 0x00;
    // ADC1MD enabled; T3MD enabled; T4MD enabled; T1MD enabled; U2MD enabled; T2MD enabled; U1MD enabled; SPI2MD enabled; SPI1MD enabled; T5MD enabled; I2C1MD enabled; 
    PMD1 = 0x00;
    // IC3MD enabled; OC1MD enabled; IC2MD enabled; OC2MD enabled; IC1MD enabled; OC3MD enabled; 
    PMD2 = 0x00;
    // RTCCMD enabled; CMPMD enabled; CRCPMD enabled; I2C2MD enabled; 
    PMD3 = 0x00;
    // EEMD enabled; CTMUMD enabled; REFOMD enabled; ULPWUMD enabled; HLVDMD enabled; 
    PMD4 = 0x00;
    // CF no clock failure; NOSC FRCPLL; SOSCEN disabled; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) (0x01));
    __builtin_write_OSCCONL((uint8_t) (0x00));
}