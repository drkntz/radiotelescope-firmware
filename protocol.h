/* File: protocol.h
 * Handle communication protocol with main PC over USB-UART bridge.
 * Authors: Zach Martin & Aaron Olsen
 * Date: 4/16/23
 * 
*/

#ifndef PROTOCOL_H
#define	PROTOCOL_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "common.h"

commands_t check_pc_commands(void); // check for new commands from UART and put them into command struct


#endif	// PROTOCOL_H

