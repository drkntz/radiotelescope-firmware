/* diagnostic.c
 * Test functions for radio-telescope board
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/22/2023
 * 
 * 
*/
#include "diagnostic.h"
#include "common.h"
#include "mcc_generated_files/adc1.h"

#define DELAY 500

void adc_test(void); // test ADC inputs
void digital_inputs(void); // digital inputs

// flip digital pins for now
void diagnostic_main(void)
{
    printf("\r\nDiagnostic menu. Hit any key to continue");
    get_char_wait_tag();
    
    //adc_test();
    digital_inputs();

    
    AZ_CONTROL1_SetHigh(); 
    printf("\r\nAZ Control1 high");
    get_char_wait_tag();

    AZ_CONTROL1_SetLow(); 
    printf("\r\nAZ Control1 low");
    get_char_wait_tag();
    
    AZ_CONTROL2_SetHigh(); 
    printf("\r\nAZ Control 2 high");
    get_char_wait_tag();
    
    AZ_CONTROL2_SetLow(); 
    printf("\r\nAZ Control 2 low");
    get_char_wait_tag();
    
    EL_CONTROL1_SetHigh(); 
    printf("\r\nEL Control1 high");
    get_char_wait_tag();
    EL_CONTROL1_SetLow(); 
    printf("\r\nEL Control1 low");
    get_char_wait_tag();
    
    EL_CONTROL2_SetHigh(); 
    printf("\r\nEL Control 2 high");
    get_char_wait_tag();
    EL_CONTROL2_SetLow(); 
    printf("\r\nEL Control 2 low");
    get_char_wait_tag();
    

}

void adc_test(void)
{
    uint16_t el, az;
    char input_char = -1;
    uint8_t i = 0; // this just shows that something is happening
    uint8_t j;
    
    printf("\r\nADC Conversion test function. Press ESC to exit");
    printf("\r\nElevation      Azimuth    index\r\n");
    

    ADC1_Initialize();
//    ADC1_InterruptDisable();
//    ADC1_InterruptFlagClear();
//    ADC1_Enable();
//    

    while(input_char != ESC)
    {
        input_char = get_char_tag();
        
        // take the mean of 10 samples
        for(j = 0; j < 10; j++)
        {
            el += ADC1_read(EL_CURRENT);
            __delay_ms(1);
        }
        el = el/10;
        
        //az = ADC1_read(AZ_CURRENT);
        printf("\r%u       %u     %u", el, az, i++);

        __delay_ms(100);
        ClrWdt();
    }
        
        
//        
//        int conversion,i=0;
//        ADC1_Initialize();
//
//        ADC1_Enable();
//        ADC1_ChannelSelect(EL_CURRENT);
//        ADC1_SoftwareTriggerEnable();
//        //Provide Delay
//        for(i=0;i <1000;i++)
//        {
//        }
//        ADC1_SoftwareTriggerDisable();
//        while(!ADC1_IsConversionComplete(EL_CURRENT));
//        conversion = ADC1_ConversionResultGet(EL_CURRENT);
//        printf("\r\n%u", conversion);
//        ADC1_Disable(); 
//        ClrWdt();
//        __delay_ms(100);
//    }
//    
//    
//        while(0)
//    {
//        ADC1_SoftwareTriggerEnable();
//        //Provide Delay
//        for(i=0;i <1000;i++)
//        {
//        }
//        ADC1_SoftwareTriggerDisable();
//        while(!ADC1_IsConversionComplete(EL_CURRENT));
//        adc_result = ADC1_ConversionResultGet(EL_CURRENT);
//        
//        printf("%u %d\r", adc_result, x++);
//        ClrWdt();
//        __delay_ms(250);
//        
//        if(get_char_tag() == ESC )
//            return;
//    }
//    ADC1_Disable(); 
}

void digital_inputs(void)
{
    printf("\r\nDigital Inputs");
    printf("\r\nAz1 Az2 El1 El2\r\n");
    AZ_ENCODER1_SetDigitalInput();
    
    while(get_char_tag() != ESC)
    {
        printf("\r %x %x %x %x", AZ_ENCODER1_GetValue(), AZ_ENCODER2_GetValue(), EL_ENCODER1_GetValue(), EL_ENCODER2_GetValue());
        ClrWdt();
        __delay_ms(200);
    }
}