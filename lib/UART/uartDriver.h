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
#include "freertos/FreeRTOS.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef enum{
    UART_OK,
    UART_FAIL   = -1
}uart_status_t;

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
 * @param data_queue queue to store data received through uart
 * 
 */
void UART_Config(uint8_t port, uint32_t baudrate, uint32_t buff_size, uint8_t tx_pin, 
                    uint8_t rx_pin, QueueHandle_t *data_queue);

/*
 * @brief Send data through the UART
 *
 * @param port UART to send data through
 * @param *data buffer of data to send
 * @param len size of buffer
 * 
*/
void UART_SendData(uint8_t port, char *data, uint16_t len);

/*
 * @brief Read data through the UART
 *
 * @param port UART to read data through
 * @param *data buffer of data to read
 * @param len size of buffer
 * 
*/
void UART_ReadData(uint8_t port, char *data, uint16_t len);

/*
 * @brief Wait for TX transfer to finish
 *
 * @param port UART to read data through
 * @return UART status
 * 
*/
uart_status_t UART_WaitTX(uint8_t port);

#endif /* UARTDRIVER_UARTDRIVER_H_ */