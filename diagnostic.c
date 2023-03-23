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
// flip digital pins for now
void diagnostic_main(void)
{
    printf("\r\nDiagnostic menu. Hit any key to cycle through pin testing");
    get_char_wait_tag();
    
    printf("\r\nel current:\r\n");
    
    uint16_t adc_result = 0;
    uint16_t i;
    uint8_t x;
    ADC1_Initialize();
    ADC1_InterruptDisable();
    ADC1_InterruptFlagClear();
    ADC1_Enable();
    

    while(1)
    {
        int conversion,i=0;
        ADC1_Initialize();

        ADC1_Enable();
        ADC1_ChannelSelect(EL_CURRENT);
        ADC1_SoftwareTriggerEnable();
        //Provide Delay
        for(i=0;i <1000;i++)
        {
        }
        ADC1_SoftwareTriggerDisable();
        while(!ADC1_IsConversionComplete(EL_CURRENT));
        conversion = ADC1_ConversionResultGet(EL_CURRENT);
        printf("\r\n%u", conversion);
        ADC1_Disable(); 
        ClrWdt();
        __delay_ms(100);
    }
    while(0)
    {
        ADC1_SoftwareTriggerEnable();
        //Provide Delay
        for(i=0;i <1000;i++)
        {
        }
        ADC1_SoftwareTriggerDisable();
        while(!ADC1_IsConversionComplete(EL_CURRENT));
        adc_result = ADC1_ConversionResultGet(EL_CURRENT);
        
        printf("%u %d\r", adc_result, x++);
        ClrWdt();
        __delay_ms(250);
        
        if(get_char_tag() == ESC )
            return;
    }
    ADC1_Disable(); 
    
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