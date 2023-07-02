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

void procotol_init(uint8_t port_uart, uint32_t baudRate );

uint16_t protocol_wait_frame( char* data, uint16_t max_size );

uint8_t protocol_set_frame(char* data, uint16_t max_size);

uint8_t protocol_send_frame(char *data, uint16_t max_size);


/*******************************************************************************
 * API
 ******************************************************************************/


#endif /* PROTOCOL_PROTOCOL_H_ */