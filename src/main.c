/*
 * Copyright 2023 Grupo MSA
 * All rights reserved.
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
#include "main.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BUF_SIZE (1024)

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

    DHT_SetGpio(DHT22_PIN);
    
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
		vTaskDelay( 2000 / portTICK_RATE_MS );
    }
}

/*******************************************************************************
 * Code - public
 ******************************************************************************/
void app_main() 
{
	xTaskCreate(DHT_task, "DHT_task", DHT_TASK_STACK, NULL, DHT_TASK_PRIORITY, NULL );
}