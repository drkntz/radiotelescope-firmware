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

/* Function prototypes: test various i/o */
void adc_test(void);     // test ADC inputs
void gpio_test(void);    // test inputs & outputs, 
void hbridge_test(void); // test hbridge
void hbridge_square_wave(void); // test hbridge

//void uart_test(void); // TODO
//void lcd_test(void); // TODO



// Top level menu
void diagnostic_main(void)
{
    char input;
    
    while(1)
    {
        printf("\r\nDiagnostic menu.\r\nOptions:");
        printf( "\r\n1 - ADC test"
                "\r\n2 - GPIO test"
                "\r\n3 - H-Bridge test"
                "\r\nESC to exit and return to previous menu"
                "\r\nEnter option: ");

        input = get_char_wait_tag();

        printf("%c\r\n", input);

        switch(input)
        {
            case '1':
                adc_test();
                break;
            case '2':
                gpio_test();
                break;
            case '3':
                hbridge_test();
                break;
            case ESC:
                return; // go back to main
        }
    }
}

/* Test ADC inputs */
void adc_test(void)
{
    uint16_t el, az;
    char input = -1;
    
    printf("\r\nADC test");
    printf("\r\nElevation      Azimuth\r\n");
    
    while(input != ESC)
    {
        input = get_char_tag();
        el = ADC1_read(EL_CURRENT);
        az = ADC1_read(AZ_CURRENT);
        printf("\r%04u       %04u", el, az);
        __delay_ms(100);
        ClrWdt();
    }
}

void gpio_test(void)
{
    char input;
    
    printf("\r\nGpio Test"
            "\r\n1- toggle STAT LED"
            "\r\n2- toggle EXT LED");
    
    printf("\r\nLED output |Encoder input    | Other input");
    printf("\r\nSTAT EXT   | AZ1 AZ2 EL1 EL2 | Btn1 Btn2 Btn3 diag\r\n");
    
    do
    {
        input = get_char_tag();
        
        switch(input)
        {
            case '1':
                nSTAT_LED_Toggle();
                break;
            case '2':
                EXT_LED_Toggle();
                break;
            default:
                break;
        }
        
        printf("\r%x  %x  | %x %x %x %x | %x %x %x %x", 
                ~nSTAT_LED_GetValue(), EXT_LED_GetValue(), AZ_ENCODER1_GetValue(),
                AZ_ENCODER2_GetValue(), EL_ENCODER1_GetValue(), EL_ENCODER2_GetValue(), 
                BUTTON1_GetValue(), BUTTON2_GetValue(), BUTTON3_GetValue(), DIAGS_GetValue());
        
        ClrWdt();
        __delay_ms(100);
    }while(input != ESC);
}

void hbridge_test(void)
{
    char input;
    
    // double check everything is off
    AZ_CONTROL1_SetLow();
    AZ_CONTROL2_SetLow();
    EL_CONTROL1_SetLow();
    EL_CONTROL2_SetLow();
    
    printf("\r\nHbridge test"
            "\r\n1- Toggle Azimuth"
            "\r\n2- Toggle Elevation"
            "\r\n3- El Control 1 Square wave"
            "\r\nAny key - stop motor"
            "\r\nESC - exit to previous menu");
    printf("\r\n--H Bridge------|--Encoders------|----Current----");
    printf("\r\nAZ1 AZ2 EL1 EL2 | AZ1 AZ2 EL1 EL2|  AZ    EL\r\n");

    do
    {
        printf("\r%x   %x   %x   %x     %x   %x   %x   %x     ", 
            AZ_CONTROL1_GetValue(), AZ_CONTROL2_GetValue(), EL_CONTROL1_GetValue(), 
            EL_CONTROL2_GetValue(), AZ_ENCODER1_GetValue(), AZ_ENCODER2_GetValue(), 
            EL_ENCODER1_GetValue(), EL_ENCODER2_GetValue());
        
        input = get_char_tag();
        
        switch(input)
        {
            case -1: // default for no input
                break;
                
            case '1': // toggle azimuth
                if(AZ_CONTROL1_GetValue() == 1) 
                {
                    AZ_CONTROL1_SetLow();
                    __delay_ms(100); // todo
                    AZ_CONTROL2_SetHigh();
                    __delay_ms(100); // todo
                }
                else if(AZ_CONTROL2_GetValue() == 1)
                {
                    AZ_CONTROL2_SetLow();
                    __delay_ms(100); // todo
                    AZ_CONTROL1_SetHigh();
                    __delay_ms(100); // todo
                }
                else
                {
                    AZ_CONTROL1_SetHigh(); // TODO
                    __delay_ms(100);
                }
                break;

            case '2': // toggle elevation
                if(EL_CONTROL1_GetValue() == 1) 
                {
                    EL_CONTROL1_SetLow();
                    __delay_ms(250); // todo
                    EL_CONTROL2_SetHigh();
                    __delay_ms(100); // todo
                }
                else if(EL_CONTROL2_GetValue() == 1)
                {
                    EL_CONTROL2_SetLow();
                    __delay_ms(250); // todo
                    EL_CONTROL1_SetHigh();
                    __delay_ms(100); // todo
                }
                else
                {
                    EL_CONTROL1_SetHigh(); // TODO
                    __delay_ms(100);
                }
                break;
                
            case '3':
                hbridge_square_wave();
                break;
                
            default: // any key kills motors
                AZ_CONTROL1_SetLow();
                AZ_CONTROL2_SetLow();
                EL_CONTROL1_SetLow();
                EL_CONTROL2_SetLow();
                break;
        }
        ClrWdt();
    }while(input != ESC);
    
    // make sure its all off
    AZ_CONTROL1_SetLow();
    AZ_CONTROL2_SetLow();
    EL_CONTROL1_SetLow();
    EL_CONTROL2_SetLow();
}

void hbridge_square_wave(void) // test hbridge
{
    AZ_CONTROL1_SetLow();
    AZ_CONTROL2_SetLow();
    EL_CONTROL1_SetLow();
    EL_CONTROL2_SetLow();
    printf("\r\n10Hz Square wave to Elevation 1");
    
    while(get_char_tag() == -1)
    {
        EL_CONTROL1_SetHigh();
        __delay_ms(50); // todo
        EL_CONTROL1_SetLow();
        __delay_ms(50); // todo
        ClrWdt();
    }
    AZ_CONTROL1_SetLow();
    AZ_CONTROL2_SetLow();
    EL_CONTROL1_SetLow();
    EL_CONTROL2_SetLow();
}