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
// uint8_t calc_crc(uint8_t *data, size_t len) ;
uint8_t crc_calc(uint8_t val, void *buf, int len);

#endif /* CRC_CRC_H_ */


// /*
//  * CRC8-CCITT, with normal polynomial; 0x07.
//  */

// #ifndef _CRC8_H_
// #define _CRC8_H_

// #include <inttypes.h>

// #ifdef __cplusplus
// extern "C" {
// #endif

// uint8_t crc8_calc(uint8_t val, void *buf, int cnt);

// #ifdef __cplusplus
// }
// #endif

// #endif