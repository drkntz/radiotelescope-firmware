/*==============================================================================
 * File: i2c1.h
 * MCC-generated i2c driver
 * Radiotelescope project
*/

#ifndef _I2C1_H
#define _I2C1_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <xc.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

typedef enum
{
    I2C1_MESSAGE_FAIL,
    I2C1_MESSAGE_PENDING,
    I2C1_MESSAGE_COMPLETE,
    I2C1_STUCK_START,
    I2C1_MESSAGE_ADDRESS_NO_ACK,
    I2C1_DATA_NO_ACK,
    I2C1_LOST_STATE
} I2C1_MESSAGE_STATUS;

typedef struct
{
    uint16_t  address;          // Bits <10:1> are the 10 bit address.
                                // Bits <7:1> are the 7 bit address
                                // Bit 0 is R/W (1 for read)
    uint8_t   length;           // the # of bytes in the buffer
    uint8_t   *pbuffer;         // a pointer to a buffer of length bytes
} I2C1_TRANSACTION_REQUEST_BLOCK;
       
void I2C1_Initialize(void);

void I2C1_MasterWrite(
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address,
                                I2C1_MESSAGE_STATUS *pstatus);


void I2C1_MasterRead(
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address,
                                I2C1_MESSAGE_STATUS *pstatus);

void I2C1_MasterTRBInsert(
                                uint8_t count,
                                I2C1_TRANSACTION_REQUEST_BLOCK *ptrb_list,
                                I2C1_MESSAGE_STATUS *pflag);

void I2C1_MasterReadTRBBuild(
                                I2C1_TRANSACTION_REQUEST_BLOCK *ptrb,
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address);                               

void I2C1_MasterWriteTRBBuild(
                                I2C1_TRANSACTION_REQUEST_BLOCK *ptrb,
                                uint8_t *pdata,
                                uint8_t length,
                                uint16_t address);                           

bool I2C1_MasterQueueIsEmpty(void);                              

bool I2C1_MasterQueueIsFull(void);             

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif //_I2C1_H