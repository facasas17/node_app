/*
 * Copyright (c) 2023, Fabiola de las Casas Escardo <fabioladelascasas@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Version: 1.0
 * 
 */
#define NODE_TEST   1
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "DHT22.h"
#include "crc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DHT22_PIN       GPIO_NUM_2  

#define UART_TXD        GPIO_NUM_17
#define UART_RXD        GPIO_NUM_16
#define UART_RTS        UART_PIN_NO_CHANGE
#define UART_CTS        UART_PIN_NO_CHANGE
#define UART_BAUDRATE   115200
#define UART_PORT       UART_NUM_2

#define RX_BUF_SIZE     1024
#define TX_BUF_SIZE     1024
#define TASK_DELAY      2000

#define NODE_TAG        0x01
#define RE_DE_ESP32PIN  GPIO_NUM_4

#define RS485_RECIEVE   0
#define RS485_TRANSMIT  1

#if NODE_TEST
#define RE_DE_CP2102PIN GPIO_NUM_5
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void UART_config(void);
static void add_CRC(char *buff);
static void mainTask(void *arg);

/*******************************************************************************
 * Variables
 ******************************************************************************/
char data_humTemp[TX_BUF_SIZE];
uint8_t rx_Node = 0;                    

/*******************************************************************************
 * Code - private
 ******************************************************************************/
static void UART_config(void)
{
    /* Configure parameters of an UART driver, communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_PORT, RX_BUF_SIZE * 2, TX_BUF_SIZE, 0, NULL, 0);
    uart_param_config(UART_PORT, &uart_config);
    uart_set_pin(UART_PORT, UART_TXD, UART_RXD, UART_RTS, UART_CTS);
}

static void add_CRC(char *buff)
{
    uint8_t crc = crc_calc(0, (uint8_t *)buff, strlen(buff));
    sprintf(buff + strlen(buff), "\r\n%02X\r\n", crc);
}

static void mainTask(void *arg)
{
    int ret;

    data_humTemp[0] = '\0';     // Reset buffer
    UART_config();
    DHT_SetGpio( DHT22_PIN );
    
	gpio_set_direction( RE_DE_ESP32PIN, GPIO_MODE_OUTPUT );

#if NODE_TEST
    gpio_set_direction( RE_DE_CP2102PIN, GPIO_MODE_OUTPUT );
    gpio_set_level( RE_DE_CP2102PIN, RS485_TRANSMIT ); 
#endif
         
    gpio_set_level( RE_DE_ESP32PIN, RS485_RECIEVE );     

    while (1) 
    {
       	gpio_set_level( RE_DE_ESP32PIN, RS485_RECIEVE );       
          
#if NODE_TEST
        gpio_set_level( RE_DE_CP2102PIN, RS485_TRANSMIT );  
#endif   

        uart_read_bytes(UART_PORT, &rx_Node, 1, TASK_DELAY);

        if( rx_Node == NODE_TAG )     // Si se recibe el mismo ID del nodo
        {
            if(data_humTemp[0] == '\0')
            {
                ret = DHT_ReadData();
                DHT_ErrorHandler(ret);

                uint16_t total = sprintf(data_humTemp, "Hum %.1f  ", DHT_GetHumidity()); // puts string into buffer
                sprintf(data_humTemp + total, "Tmp %.1f", DHT_GetTemperature()); // puts string into buffer
            
                add_CRC(( char *)data_humTemp);
            }

       	    gpio_set_level( RE_DE_ESP32PIN, RS485_TRANSMIT );       

#if NODE_TEST
            gpio_set_level( RE_DE_CP2102PIN, RS485_RECIEVE );  
#endif

            if( ESP_OK == uart_wait_tx_done(UART_PORT, TASK_DELAY))
            {
                uart_write_bytes(UART_PORT, data_humTemp, strlen(data_humTemp));

                data_humTemp[0] = '\0';     // Reset buffer
            }

            rx_Node = 0;   // Limpio el valor recibido.
        }
        else
        {

#if NODE_TEST
       	    gpio_set_level( RE_DE_ESP32PIN, RS485_TRANSMIT );       
            gpio_set_level( RE_DE_CP2102PIN, RS485_RECIEVE );  
            uart_write_bytes(UART_PORT, "Sin pedido de data\n", strlen("Sin pedido de data\n"));
#endif 
           
        }

        vTaskDelay(TASK_DELAY / portTICK_PERIOD_MS);
    }
}

/*******************************************************************************
 * Code - public
 ******************************************************************************/
void app_main(void)
{
    xTaskCreate(mainTask, "main_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
}
