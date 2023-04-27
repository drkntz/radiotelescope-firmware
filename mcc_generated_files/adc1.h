/*==============================================================================
 * File: adc1.h
 * ADC driver for Pic24. This replaces the MCC one that did not work
 * Authors: Zach Martin & Aaron Olsen
 * Date: 3/23/2023
 * 
 * with Vref = 4.096V, we get 4.096/2^12 = 1mV per ADC step
*/

#ifndef _ADC1_H
#define _ADC1_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif


#define ADC1_SCAN_MODE_SELECTED true

// these are the channels that we could read
typedef enum 
{
    EL_CURRENT,//Channel Name:AN4   Assigned to:Shared Channel
    AZ_CURRENT,//Channel Name:AN5   Assigned to:Shared Channel
    CHANNEL_CTMU_temperature_sensor_input,//Channel Name:CTMU temperature sensor input   Assigned to:Shared Channel
    CHANNEL_CTMU,//Channel Name:CTMU   Assigned to:Shared Channel
    CHANNEL_VBG,//Channel Name:VBG   Assigned to:Shared Channel
    CHANNEL_Lower_guardband_rail,//Channel Name:Lower guardband rail   Assigned to:Shared Channel
    CHANNEL_Upper_guardband_rail,//Channel Name:Upper guardband rail   Assigned to:Shared Channel
    CHANNEL_AVSS,//Channel Name:AVSS   Assigned to:Shared Channel
    CHANNEL_AVDD,//Channel Name:AVDD   Assigned to:Shared Channel
} ADC1_CHANNEL;

void ADC1_Initialize (void);

uint16_t ADC1_read(ADC1_CHANNEL channel); // read ADC channel

// this might be useful to save for later TODO
inline static uint16_t ADC1_ConversionResultGet( ADC1_CHANNEL channel )
{
    uint16_t result;

    switch(channel)
    {
        case EL_CURRENT:
                result = ADC1BUF0;
                break;
        case AZ_CURRENT:
                result = ADC1BUF1;
                break;
        case CHANNEL_CTMU_temperature_sensor_input:
                result = ADC1BUF2;
                break;
        case CHANNEL_CTMU:
                result = ADC1BUF3;
                break;
        case CHANNEL_VBG:
                result = ADC1BUF4;
                break;
        case CHANNEL_Lower_guardband_rail:
                result = ADC1BUF5;
                break;
        case CHANNEL_Upper_guardband_rail:
                result = ADC1BUF6;
                break;
        case CHANNEL_AVSS:
                result = ADC1BUF7;
                break;
        case CHANNEL_AVDD:
                result = ADC1BUF8;
                break;
        default:
                break;
    }

    return result;
}

// this might be useful to save for later TODO
inline static bool ADC1_IsConversionComplete(ADC1_CHANNEL channel)
{
    bool status;

    status = AD1CON1bits.DONE;

    return status;
}

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif //_ADC1_H