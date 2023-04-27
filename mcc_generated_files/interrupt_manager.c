/*==============================================================================
 * File: interrupt_manager.c
 * MCC-generated interrupt interface
 * Radiotelescope project
*/

#include <xc.h>

void INTERRUPT_Initialize (void)
{
    //    MICI: MI2C1 - I2C1 Master Events
    //    Priority: 1
        IPC4bits.MI2C1IP = 1;
    //    SICI: SI2C1 - I2C1 Slave Events
    //    Priority: 1
        IPC4bits.SI2C1IP = 1;
    //    CNI: CN -  Change Notification Interrupt
    //    Priority: 1
        IPC4bits.CNIP = 1;
}