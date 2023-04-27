/*==============================================================================
 * File: diagnostic.h
 * Test functions for radio-telescope board
 * Authors: Zach Martin & Aaron Olsen
 * Date: 3/22/2023
*/

#ifndef DIAGNOSTIC_H
#define	DIAGNOSTIC_H

#include <xc.h> // include processor files - each processor file is guarded.  

void diagnostic_main(void); // main diags function
void _encoder_test_callback(void); // Pulse Counter

#endif	/* XC_HEADER_TEMPLATE_H */