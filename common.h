/* common.h
 * Common libraries and constants for radio-telescope control system project
 * Authors: Zach Martin, Aaron Olsen, Loc pham
 * Date: 3/9/2023
*/

#ifndef COMMON_H
#define	COMMON_H

// FCY is used for delays. Make sure its defined here before libpic30.h is included
#ifndef FCY 
#define FCY (_XTAL_FREQ / 2)
#endif 

#include <xc.h> // include processor files - each processor file is guarded.
#include <libpic30.h>                   // also for delays and other stuff
#include <stdio.h>                      // for printf
#include "mcc_generated_files/clock.h"  // for delay functions
#include "p24FV32KA304.h"               // not sure if this has to be explicitly included? TODO
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"

// TODO: put program constants here
char get_char_wait_tag(void);

#endif	// COMMON_H

