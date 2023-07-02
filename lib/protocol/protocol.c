/*
 * Copyright (c) 2023, Fabiola de las Casas Escardo <fabioladelascasas@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Version: 1.0
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "protocol.h"
#include "crc.h"
#include "uartDriver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code - private
 ******************************************************************************/

/*******************************************************************************
 * Code - public
 ******************************************************************************/

// uint8_t Protocol_SetFrame(char* data, uint16_t max_size)
// {

// }

// Frame structure


// Function to set up the frame
void setUartFrame(UartFrame *frame, uint8_t address, uint8_t actionCode, uint8_t flagStatus, uint32_t payload, uint8_t crc) 
{
    frame->address = address;
    frame->actionCode = actionCode;
    frame->flagStatus = flagStatus;
    frame->payload = payload;
    frame->crc = crc;
}

uint16_t build_Frame(char *data_buff, UartFrame *frame)
{
    uint16_t len_data;

    len_data = sprintf(data_buff, "%X%X%X%X\n", frame->address, frame->actionCode, frame->flagStatus, frame->crc);
    return len_data;
}
                
// Function to send a frame over UART
void sendUartFrame(UartFrame *frame) 
{
    //uart_write_bytes(UART_PORT, (const char *)frame, sizeof(UartFrame));
}


