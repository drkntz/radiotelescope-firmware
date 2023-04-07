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
#include <stdbool.h>

/* Function prototypes: test various i/o */
void adc_test(void);     // test ADC inputs
void gpio_test(void);    // test inputs & outputs, 
void hbridge_test(void); // test hbridge
void hbridge_square_wave(void); // test hbridge
void encoder_test(void); // test rotary encoders

// helper for encoder_test()
void _encoder_test_callback(void);

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
                "\r\n4 - Read encoder interrupts"
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
            case '4':
                encoder_test();
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
        
        printf("\r %x    %x    |  %x   %x   %x   %x  |   %x   %x   %x     %x", 
                (0x01 & ~nSTAT_LED_GetValue()), EXT_LED_GetValue(), AZ_ENCODER1_GetValue(),
                AZ_ENCODER2_GetValue(), EL_ENCODER1_GetValue(), EL_ENCODER2_GetValue(), 
                BUTTON1_GetValue(), BUTTON2_GetValue(), BUTTON3_GetValue(), DIAGS_GetValue());
        
        ClrWdt();
        __delay_ms(100);
    }while(input != ESC);
}

void hbridge_test(void)
{
    char input;
    uint16_t el_current = 10;
    uint16_t az_current = 10;
    
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
        printf("\r %x   %x   %x   %x  |  %x   %x   %x   %x | %4u  %4u", 
            AZ_CONTROL1_GetValue(), AZ_CONTROL2_GetValue(), EL_CONTROL1_GetValue(), 
            EL_CONTROL2_GetValue(), AZ_ENCODER1_GetValue(), AZ_ENCODER2_GetValue(), 
            EL_ENCODER1_GetValue(), EL_ENCODER2_GetValue(), az_current, el_current);
        
        el_current = 3*el_current/4 + ADC1_read(EL_CURRENT)/4;
        az_current = 3*az_current/4 + ADC1_read(AZ_CURRENT)/4;
        
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
        __delay_ms(100);
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
    printf("\r\n0.5Hz Square wave to Elevation 1");
    
    while(get_char_tag() == -1)
    {
        EL_CONTROL1_SetHigh();
        __delay_ms(1000); // todo
        EL_CONTROL1_SetLow();
        __delay_ms(1000); // todo
        ClrWdt();
    }
    AZ_CONTROL1_SetLow();
    AZ_CONTROL2_SetLow();
    EL_CONTROL1_SetLow();
    EL_CONTROL2_SetLow();
}


// file scope, count encoder pulses
uint16_t az1_pulse, az2_pulse, el1_pulse, el2_pulse; // count the changes that are happening. Increment on change
bool az1, az2, el1, el2; // use these to store the previous state. TODO could use flags in a single byte
       
// helper for encoder_test()
void _encoder_test_callback(void)
{
    // increment the encoder pulse variable if 
    // the input has changed.
    bool trash = AZ_ENCODER1_GetValue(); 
    if(az1 != trash)
    {
        az1 = trash;
        az1_pulse ++;
    }
    
    trash = AZ_ENCODER2_GetValue();
    if(az2 != trash)
    {
        az2 = trash;
        az2_pulse ++;
    }
    
    trash = EL_ENCODER1_GetValue();
    if(el1 != trash)
    {
        el1 = trash;
        el1_pulse ++;
    }
    
    trash = EL_ENCODER2_GetValue();
    if(el2 != trash)
    {
        el2 = trash;
        el2_pulse ++;
    }
    
    nSTAT_LED_Toggle();
}

// test rotary encoder interrupts
void encoder_test(void)
{
    char input = -1;
    az1_pulse = 0; 
    az2_pulse = 0; 
    el1_pulse = 0;
    el2_pulse = 0;
    
    // set initial pin state
    az1 = AZ_ENCODER1_GetValue();
    az2 = AZ_ENCODER2_GetValue();
    el1 = EL_ENCODER1_GetValue();
    el2 = EL_ENCODER2_GetValue();
    
    CN_SetInterruptHandler(_encoder_test_callback); // set callback for CN interrupts
        
    printf("\r\nEncoder interrupt test");
    printf("\r\nPulse counts increment with each change of state");
    printf("\r\nPulses   |   Pin state ");// note hte pin state is not actually from a port read but from the previous state stored in the interrupt callback 
    printf("\r\naz1 az2 el1 el2   |  az1 az2 el1 el2\r\n");
    
    while(input != ESC)
    {
        input = get_char_tag();
        printf("\r%x %x %x %x | %x %x %x %x", az1_pulse, az2_pulse, el1_pulse, el2_pulse, az1, az2, el1, el2);
        __delay_ms(100);
        ClrWdt();
    }
}


