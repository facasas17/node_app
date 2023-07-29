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
static int add_CRC(char *buff);
static uint8_t check_CRC(char *buff);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code - private
 ******************************************************************************/
static int add_CRC(char *buff)
{
    uint16_t size_buff;
    uint8_t crc;

    size_buff = strlen(buff);
    crc = crc_calc(0, (uint8_t *)buff, size_buff);

    return sprintf(buff + size_buff, "%d", crc);
}

static uint8_t check_CRC(char *buff)
{
    uint16_t size_buff;

    size_buff = strlen(buff);
    return crc_calc(0, (uint8_t *)buff, size_buff-1);
}

/*******************************************************************************
 * Code - public
 ******************************************************************************/

// Function to set the data on the frame
void protocol_setFrame(protocol_frame_t *frame, uint8_t address, uint8_t actionCode, uint8_t flagStatus, 
                        uint16_t temp, uint16_t hum) 
{
    frame->address = address;
    frame->actionCode = actionCode;
    frame->flagStatus = flagStatus;
    frame->payload = ((uint32_t)temp << 16) | hum;
}

// Function to build the data buffer to send
uint16_t protocol_buildFrame(char *data_buff, protocol_frame_t *frame)
{
    uint16_t len_data;
    memset(data_buff, 0, strlen(data_buff));      // Reset buffer
    
    len_data = sprintf(data_buff, "%d%d%d%lu", frame->address, frame->actionCode, frame->flagStatus, frame->payload);
    len_data += add_CRC(data_buff);
    
    return len_data;
}
                
crc_status_t protocol_readFrame(char *data_buff, protocol_frame_t *frame)
{
    uint8_t crc;

    frame->address = data_buff[0];
    frame->actionCode = data_buff[1] >> 4;
    frame->flagStatus = data_buff[1] & 0x0F;
    memcpy(&(frame->payload), data_buff + 2, 4);
    crc = data_buff[6];

    if(crc == check_CRC(data_buff))
    {
        return CRC_OK;
    }
    else
    {
        return CRC_ERROR;
    }
}
