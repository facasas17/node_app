/*
 * Copyright (c) 2023, Fabiola de las Casas Escardo <fabioladelascasas@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Version: 1.0
 * 
 */

#ifndef CRC_CRC_H_
#define CRC_CRC_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CRC_POLY        0x07

/*******************************************************************************
 * API
 ******************************************************************************/
uint8_t calc_crc(uint8_t *data, size_t len) ;

#endif /* CRC_CRC_H_ */