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
#if NODE_TEST
#include "driver/uart.h"
#include "driver/gpio.h"
#endif

#include "DHT22.h"
#include "crc.h"
#include "uartDriver.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DHT22_PIN       GPIO_NUM_2  

#define RE_DE_ESP32PIN  GPIO_NUM_4

#define UART_TXD        GPIO_NUM_17
#define UART_RXD        GPIO_NUM_16
#define UART_BAUDRATE   115200
#define UART_PORT       UART_NUM_2

#define BUF_SIZE        1024
#define TASK_DELAY      500

#if NODE_TEST
#define RE_DE_CP2102PIN GPIO_NUM_5
#define RS485_RECIEVE   0
#define RS485_TRANSMIT  1
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static int add_CRC(char *buff);
static void mainTask(void *arg);

/*******************************************************************************
 * Variables
 ******************************************************************************/
char data_humTemp[BUF_SIZE];
QueueHandle_t uart_queue;
/*******************************************************************************
 * Code - private
 ******************************************************************************/
int add_CRC(char *buff)
{
    uint16_t size_buff;
    uint8_t crc;

    size_buff = strlen(buff);
    crc = crc_calc(0, (uint8_t *)buff, size_buff);

    return sprintf(buff + size_buff, "\r\n%02X\r\n", crc);
}

static void mainTask(void *arg)
{
    int ret;
    uint16_t len_data = 0;
    uart_event_t event;

    memset(data_humTemp, 0, BUF_SIZE);      // Reset buffer

    UART_Config(UART_PORT, UART_BAUDRATE, BUF_SIZE, UART_TXD, UART_RXD, &uart_queue);
    DHT_SetGpio( DHT22_PIN );
    
    RS485_ConfigGPIO(RE_DE_ESP32PIN);

#if NODE_TEST
    gpio_set_direction( RE_DE_CP2102PIN, GPIO_MODE_OUTPUT );
    gpio_set_level( RE_DE_CP2102PIN, RS485_TRANSMIT ); 
#endif
           
    while (1) 
    {
        RS485_EnableReceiveData();
          
#if NODE_TEST
        gpio_set_level( RE_DE_CP2102PIN, RS485_TRANSMIT );  
#endif   

        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY))
        {      
            if (event.type == UART_PATTERN_DET)
            {
                ret = DHT_ReadData();
                DHT_ErrorHandler(ret);

                len_data = sprintf(data_humTemp, "Hum %.1f  ", DHT_GetHumidity());
                len_data += sprintf(data_humTemp + len_data, "Tmp %.1f", DHT_GetTemperature()); 
                    
                len_data += add_CRC(( char *)data_humTemp);

                RS485_EnableSendData();

        #if NODE_TEST
                gpio_set_level( RE_DE_CP2102PIN, RS485_RECIEVE );  
        #endif

                if( UART_OK == UART_WaitTX(UART_PORT))
                {
                    UART_SendData(UART_PORT, data_humTemp, len_data);
                    memset(data_humTemp, 0, BUF_SIZE);      // Reset buffer
                }
            }
            else
            {

        #if NODE_TEST
                gpio_set_level( RE_DE_ESP32PIN, RS485_TRANSMIT );       
                gpio_set_level( RE_DE_CP2102PIN, RS485_RECIEVE );  
                UART_SendData(UART_PORT, "Sin pedido de data\n", strlen("Sin pedido de data\n"));
        #endif 
                
            } 
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

