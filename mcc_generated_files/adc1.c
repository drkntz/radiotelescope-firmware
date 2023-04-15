/* adc1.c
 * ADC driver for Pic24. This replaces the MCC one that did not work
 * Authors: Zach Martin & Aaron Olsen
 * Date: 3/23/2023
 * 
 * with Vref = 4.096V, we get 4.096/2^12 = 1mV per ADC step
 * 
*/

#include "adc1.h"
#include "../common.h" // for delay functions

void ADC1_Initialize (void)
{
    // RB2 = AN4 = Azimuth motor current sensor
    TRISBbits.TRISB2 = 1;	    // input
    ANSBbits.ANSB2 = 1;		    // analog

    // RB3 = AN5 = Elevation Motor current sensor
    TRISBbits.TRISB3 = 1;	    // input
    ANSBbits.ANSB3 = 1;		    // analog
    
    AD1CON5bits.BGREQ = 1;      // enable band gap voltage reference
    AD1CON2bits.PVCFG = 0b11; // Internal VRH2. VRH1 = 2*bandgap = 2.048V. VRH2 = 4*bandgap = 4.096V
    //AD1CON2bits.PVCFG = 0b00;   // set Vref to VDD TODO: vdd is externally supplied and not stable. Could use internal vref?
    AD1CHSbits.CH0NA = 0;       // gnd reference = vss
    AD1CON1bits.MODE12 = 1;     // 12 bit results
    AD1CON1bits.FORM = 0;       // unsigned decimal, right justified
    AD1CON3bits.ADCS = 0x1F;    // Select Tad = 32*Tcy.
    AD1CON3bits.SAMC = 0x1F;    // Set sample time = 31*Tad

    /* How to get results:
     * Select channel with 
     * AD1CHSbits.CH0SA = 0x05; // this is AN5 aka elevation
     * AD1CHSbits.CH0SA = 0x04; // this is AN4 aka azimuth 
     * read results from ADC1BUFn and store in uint16_t
     * convert result to a physical value (aka current)
     */


    AD1CON1bits.SSRC = 7;   // auto convert
    AD1CON1bits.ASAM = 1;   // sample immediately after last conversion
    AD1CSSL = 0;            // scanning not used

    //AD1CON5bits.BGREQ = 1; // enable band gap voltage reference when ADC is on TODO
        // TODO these may be needed later
    //AD1CON2bits.PVCFG = 0b10; // Internal VRH1. VRH1 = 2*bandgap = 2.048V. VRH2 = 4*bandgap = 4.096V
    //AD1CON5bits.BGREQ = 1; // enable band gap voltage ref
    AD1CON1bits.ADON = 1; // turn on the ADC
}

// Returns a raw ADC value with an input of channel ADC1_CHANNEL enum type
uint16_t ADC1_read(ADC1_CHANNEL channel) 
{
    uint16_t result;
    
    AD1CHSbits.CH0SA = 0x00; // read VSS
    __delay_us(250);
    while(!AD1CON1bits.DONE);   // Wait for conversion to complete
    result = ADC1BUF0;          // Dummy read
    
    // select S&H channel for ADC1 based on input parameter
    switch(channel)
    {
        case EL_CURRENT:
            AD1CHSbits.CH0SA = 0x04; // select AN4
            break;
        case AZ_CURRENT:
            AD1CHSbits.CH0SA = 0x05; // select AN5
            break;
        default: // TODO: there are other channels in the enum... 
                 // we could just remove them or actually add cases for them somewhere here
            return 0; // just early return nothing, TODO: could raise an error?
    }
    
    __delay_us(250); // wait for ADC to switch channels and settle

    while(!AD1CON1bits.DONE);   // Wait for conversion to complete
    result = ADC1BUF0;          // Dummy read
    __delay_us(250); // just to be on the safe side TODO
    
    while(!AD1CON1bits.DONE);   // Wait for conversion to complete
    result = ADC1BUF0;          //read ADC
    
    return result;
}
