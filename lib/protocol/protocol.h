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
typedef struct {
    uint8_t address;
    uint8_t actionCode : 4;
    uint8_t flagStatus : 4;
    uint32_t payload;
} protocol_frame_t;

/*******************************************************************************
 * API
 ******************************************************************************/
void protocol_setFrame(protocol_frame_t *frame, uint8_t address, uint8_t actionCode, uint8_t flagStatus, 
                        uint16_t temp, uint16_t hum); 
uint16_t protocol_buildFrame(char *data_buff, protocol_frame_t *frame);
void protocol_readFrame();

#endif /* PROTOCOL_PROTOCOL_H_ */