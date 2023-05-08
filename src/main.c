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
#include "driver/uart.h"
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
#define BUF_SIZE (1024)

#define DHT22_GPIO      GPIO_NUM_4  

#define UART_GPIO_TXD   GPIO_NUM_17
#define UART_GPIO_RXD   GPIO_NUM_16
#define UART_PORT       UART_NUM_2
#define UART_BAUDRATE   115200  

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
char data_humTemp[BUF_SIZE];

/*******************************************************************************
 * Code - private
 ******************************************************************************/
void DHT_task(void *arg)
{
    char data_hum[10];
    char data_temp[12];
    int res;

    DHT_SetGpio(DHT22_GPIO);
    
    while(1)
    {
        res = DHT_ReadData();

		DHT_ErrorHandler(res);

        snprintf(data_hum, 10, "Hum %.1f\n", DHT_GetHumidity()); // puts string into buffer
        snprintf(data_temp, 12, "Tmp %.1f\r\n", DHT_GetTemperature()); // puts string into buffer
        strcat(( char *)data_humTemp, ( char *)data_hum);
        strcat(( char *)data_humTemp, ( char *)data_temp);
		// -- wait at least 2 sec before reading again ------------
		// The interval of whole process must be beyond 2 seconds !! 
		vTaskDelay( 2000 / portTICK_PERIOD_MS );
    }
}

void UART_task(void *arg)
{
    UART_Config(UART_PORT, UART_BAUDRATE, BUF_SIZE, UART_GPIO_TXD, UART_GPIO_RXD);

    while (1) 
    {
        uart_write_bytes(UART_NUM_2, data_humTemp, strlen(data_humTemp));
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

/*******************************************************************************
 * Code - public
 ******************************************************************************/
void app_main() 
{
	xTaskCreate(DHT_task, "DHT_task", DHT_TASK_STACK, NULL, DHT_TASK_PRIORITY, NULL );
    xTaskCreate(UART_task, "UART_task", UART_TASK_STACK, NULL, UART_TASK_PRIORITY, NULL );

    vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler */
	while (1)
	{
		/* Infinite loop */
	}
}