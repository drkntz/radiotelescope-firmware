/* file: motor.h
 * Motor control file for radio-telescope project
 * Authors: Zach Martin & Aaron Olsen
 * Date: 4/20/2023
*/


#ifndef MOTOR_H
#define	MOTOR_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "common.h"

void update_motors_new(void); // public function for updating in main loop

#endif	// MOTOR_H

