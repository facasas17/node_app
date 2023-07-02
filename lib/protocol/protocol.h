/*
 * Copyright (c) 2023, Fabiola de las Casas Escardo <fabioladelascasas@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Version: 1.0
 * 
 */

#ifndef PROTOCOL_PROTOCOL_H_
#define PROTOCOL_PROTOCOL_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
/*******************************************************************************
 * Definitions
******************************************************************************/

// void Protocol_Init(uint8_t port_uart, uint32_t baudRate );

// uint16_t protocol_wait_frame( char* data, uint16_t max_size );

// uint8_t Protocol_SetFrame(char* data, uint16_t max_size);

// uint8_t Protocol_SendFrame(char *data, uint16_t max_size);

typedef struct {
    uint8_t address;
    uint8_t actionCode : 4;
    uint8_t flagStatus : 4;
    uint32_t payload;
    uint8_t crc;
} UartFrame;
/*******************************************************************************
 * API
 ******************************************************************************/
void setUartFrame(UartFrame *frame, uint8_t address, uint8_t actionCode, uint8_t flagStatus, uint32_t payload, uint8_t crc) ;
void sendUartFrame(UartFrame *frame) ;
uint16_t build_Frame(char *data_buff, UartFrame *frame);

#endif /* PROTOCOL_PROTOCOL_H_ */