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
#include "uartDriver.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_RTS        UART_PIN_NO_CHANGE
#define UART_CTS        UART_PIN_NO_CHANGE

#define NODE_TAG        0x01

#define UART_DELAY      500
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

void UART_Config(uint8_t port, uint32_t baudrate, uint32_t buff_size, uint8_t tx_pin, uint8_t rx_pin, 
                    QueueHandle_t *data_queue)
{
    /* Configure parameters of an UART driver, communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(port, buff_size * 2, buff_size, 4, data_queue, 0);
    uart_param_config(port, &uart_config);
    uart_set_pin(port, tx_pin, rx_pin, UART_RTS, UART_CTS);

    uart_enable_pattern_det_baud_intr(port, NODE_TAG, 1, 9, 0, 0);
    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(port, 2);
}

void UART_SendData(uint8_t port, char *data, uint16_t len)
{
    uart_write_bytes(port, data, len);
}

void UART_ReadData(uint8_t port, char *data, uint16_t len)
{
    uart_read_bytes(port, data, len, UART_DELAY);
}

uart_status_t UART_WaitTX(uint8_t port)
{
    return uart_wait_tx_done(port, UART_DELAY);
}