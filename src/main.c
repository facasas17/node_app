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
#include "driver/gpio.h"
#include <string.h>

#include <stdio.h>
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "DHT22.h"
#include "uartDriver.h"
#include "main.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BUF_SIZE        (1024)

#define DHT22_GPIO      GPIO_NUM_4  

#define UART_GPIO_TXD   GPIO_NUM_17
#define UART_GPIO_RXD   GPIO_NUM_16
#define UART_PORT       UART_NUM_2
#define UART_BAUDRATE   115200  

#define COMMAND     "NODE1"
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
QueueHandle_t uart_queue;    // Queue to hold UART data

/*******************************************************************************
 * Code - private
 ******************************************************************************/
uint8_t calc_crc(uint8_t *data, size_t len) 
{
    uint8_t crc = 0;

    for (size_t i = 0; i < len; i++) 
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) 
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }

    return crc;
}

void add_CRC(char *buff)
{
    uint8_t crc = calc_crc((uint8_t *)buff, strlen(buff));
    sprintf(buff + strlen(buff), "%02X\r\n", crc);
}

void read_DHTdata(char *buff)
{
    int res;
    float temperature = 0.0f, humidity = 0.0f;

    res = DHT_ReadData();
	DHT_ErrorHandler(res);
    
    humidity = DHT_GetHumidity();
    temperature = DHT_GetTemperature();

    sprintf(buff, "NODE1,%.2f,%.2f,", humidity, temperature);
}

void UART_task(void *arg)
{
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    int len;
    char data_humTemp[BUF_SIZE];

    while (1) 
    {
        len = uart_read_bytes(UART_PORT, data, BUF_SIZE - 1, pdMS_TO_TICKS(100));

        if (len > 0) 
        {
            data[len] = '\0';

            if (strcmp((char *)data, COMMAND) == 0) 
            {
                read_DHTdata(data_humTemp);
                add_CRC(data_humTemp);
                UART_SendData(UART_PORT, data_humTemp, strlen(data_humTemp));
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

/*******************************************************************************
 * Code - public
 ******************************************************************************/
void app_main() 
{
    UART_Config(UART_PORT, UART_BAUDRATE, BUF_SIZE, UART_GPIO_TXD, UART_GPIO_RXD, &uart_queue);
    DHT_SetGpio(DHT22_GPIO);

    xTaskCreate(UART_task, "UART_task", UART_TASK_STACK, NULL, UART_TASK_PRIORITY, NULL );

    vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler */
	while (1)
	{
		/* Infinite loop */
	}
}