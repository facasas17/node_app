/*
 * Copyright (c) 2023, Fabiola de las Casas Escardo <fabioladelascasas@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Version: 1.0
 * 
 */

#ifndef UARTDRIVER_UARTDRIVER_H_
#define UARTDRIVER_UARTDRIVER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "driver/uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * API
 ******************************************************************************/
/* 
 * @brief Configure parameters of an UART driver, communication pins and install the driver 
 * 
 * @param port UART to configure
 * @param baudrate
 * @param buff_size size of buffer for TX and RX
 * @param tx_pin GPIO for TX data
 * @param rx_pin GPIO for RX data
 * 
 */
void UART_Config(uint8_t port, uint32_t baudrate, uint32_t buff_size, uint8_t tx_pin, uint8_t rx_pin);

#endif /* UARTDRIVER_UARTDRIVER_H_ */