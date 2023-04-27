/*==============================================================================
 * File: system.c
 * MCC-generated (and human modified) system file
 * Radiotelescope project
*/

// These are the processor configuration bits
// FBS
#pragma config BWRP = OFF    //Boot Segment Write Protect->Disabled
#pragma config BSS = OFF    //Boot segment Protect->No boot program flash segment

// FGS
#pragma config GWRP = OFF    //General Segment Write Protect->General segment may be written
#pragma config GSS0 = OFF    //General Segment Code Protect->No Protection

// FOSCSEL
#pragma config FNOSC = FRCPLL    //Oscillator Select->Fast RC Oscillator with Postscaler and PLL Module (FRCDIV+PLL)
#pragma config SOSCSRC = 0      //SOSC Source Type->Do not use crystal oscillator, just use GPIO
#pragma config LPRCSEL = HP    //LPRC Oscillator Power and Accuracy->High Power, High Accuracy Mode
#pragma config IESO = ON    //Internal External Switch Over bit->Internal External Switchover mode enabled (Two-speed Start-up enabled)

// FOSC
#pragma config POSCMOD = NONE    //Primary Oscillator Configuration bits->Primary oscillator disabled
#pragma config OSCIOFNC = OFF    //CLKO Enable Configuration bit->CLKO output disabled
#pragma config POSCFREQ = HS    //Primary Oscillator Frequency Range Configuration bits->Primary oscillator/external clock input frequency greater than 8MHz
#pragma config SOSCSEL = SOSCHP    //SOSC Power Selection Configuration bits->Secondary Oscillator configured for high-power operation
#pragma config FCKSM = CSDCMD    //Clock Switching and Monitor Selection->Both Clock Switching and Fail-safe Clock Monitor are disabled

// FWDT
#pragma config WDTPS = PS2048    //Watchdog Timer Postscale Select bits->1:2048
#pragma config FWPSA = PR128    //WDT Prescaler bit->WDT prescaler ratio of 1:128
#pragma config FWDTEN = ON    //Watchdog Timer Enable bits->WDT enabled in hardware
#pragma config WINDIS = OFF    //Windowed Watchdog Timer Disable bit->Standard WDT selected(windowed WDT disabled)

// FPOR
#pragma config BOREN = BOR3    //Brown-out Reset Enable bits->Brown-out Reset enabled in hardware, SBOREN bit disabled
#pragma config LVRCFG = OFF    //Low Voltage Regulator Configuration bit->Low Voltage regulator is not available
#pragma config PWRTEN = ON    //Power-up Timer Enable bit->PWRT enabled
#pragma config I2C1SEL = PRI    //Alternate I2C1 Pin Mapping bit->Use Default SCL1/SDA1 Pins For I2C1
#pragma config BORV = V20    //Brown-out Reset Voltage bits->Brown-out Reset set to lowest voltage (2.0V)
#pragma config MCLRE = ON    //MCLR Pin Enable bit->RA5 input pin disabled,MCLR pin enabled

// FICD
#pragma config ICS = PGx1    //ICD Pin Placement Select bits->EMUC/EMUD share PGC1/PGD1

// FDS
#pragma config DSWDTPS = DSWDTPSF    //Deep Sleep Watchdog Timer Postscale Select bits->1:2,147,483,648 (25.7 Days)
#pragma config DSWDTOSC = LPRC    //DSWDT Reference Clock Select bit->DSWDT uses Low Power RC Oscillator (LPRC)
#pragma config DSBOREN = ON    //Deep Sleep Zero-Power BOR Enable bit->Deep Sleep BOR enabled in Deep Sleep
#pragma config DSWDTEN = ON    //Deep Sleep Watchdog Timer Enable bit->DSWDT enabled

#include "pin_manager.h"
#include "clock.h"
#include "system.h"
#include "i2c1.h"
#include "uart1.h"
#include "uart2.h"
#include "interrupt_manager.h"
#include "traps.h"
#include "adc1.h"
#include "tmr1.h"
#include "../LiquidCrystal_pic.h"
#include "../common.h"

// This is the main system setup program, called at the beginning of main()
void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    CLOCK_Initialize();
    INTERRUPT_Initialize();
    UART2_Initialize();
    I2C1_Initialize();
    UART1_Initialize();
    ADC1_Initialize();
    TMR1_Initialize();
    TMR1_Period16BitSet(0xFFFF); // use the full timer period
    TMR1_Start();
    
    // Make sure everything is off
    AZ_CONTROL1_SetLow();
    AZ_CONTROL2_SetLow();
    EL_CONTROL1_SetLow();
    EL_CONTROL2_SetLow();
    
    motor.alt.dir = MOTOR_STOP;
    motor.az.dir = MOTOR_STOP;
    command.command = CMD_STOP;
    
    INTERRUPT_GlobalEnable(); 
    
    lcd_create(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display 
    lcd_init();
    lcd_backlight();
    lcd_clear();
    CNPU2bits.CN26PUE = 1; // DIAGS is weak pull up
    EXT_LED_SetHigh(); // For now, pushbutton ground is via external LED
}