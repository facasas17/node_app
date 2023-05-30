/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "DHT22.h"

/**
 * This is an example which echos any data it receives on UART1 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART2
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

#define ECHO_TEST_TXD  (GPIO_NUM_17)
#define ECHO_TEST_RXD  (GPIO_NUM_16)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)
#define DHT22_PIN       GPIO_NUM_4  

#define BUF_SIZE (1024)

char Txdata[BUF_SIZE];// = (char*) malloc(BUF_SIZE);
char data_humTemp[BUF_SIZE];

uint8_t calc_crc(uint8_t *data, size_t len) 
{
    uint8_t crc = 0;

    for (size_t i = 0; i < len; i++) 
    {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) 
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }

    return crc;
}

void add_CRC(char *buff)
{
    uint8_t crc = calc_crc((uint8_t *)buff, strlen(buff));
    sprintf(buff + strlen(buff), "\r\n%02X\r\n", crc);
}

static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);

    // Configure a temporary buffer for the incoming data
    // char* Txdata = (char*) malloc(BUF_SIZE);
    
    DHT_SetGpio( DHT22_PIN );
    
    while (1) {
        data_humTemp[0] = '\0';
        int ret = DHT_ReadData();
		DHT_ErrorHandler(ret);
        sprintf(data_humTemp + strlen(data_humTemp), "Hum %.1f  ", DHT_GetHumidity()); // puts string into buffer
        sprintf(data_humTemp + strlen(data_humTemp), "Tmp %.1f", DHT_GetTemperature()); // puts string into buffer
        //snprintf(data_hum, 10, "Hum %.1f  ", DHT_GetHumidity()); // puts string into buffer
        //snprintf(data_temp, 12, "Tmp %.1f\r\n", DHT_GetTemperature()); // puts string into buffer
        // strcat(( char *)data_humTemp, ( char *)data_hum);
        // strcat(( char *)data_humTemp, ( char *)data_temp);
        
        add_CRC(( char *)data_humTemp);
        uart_write_bytes(UART_NUM_2, data_humTemp, strlen(data_humTemp));
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void DHT_task(void *arg)
{
    char data_hum[10];
    char data_temp[12];
    //uint16_t num = 0;
    
    DHT_SetGpio( DHT22_PIN );
    
    while(1)
    {
        // sprintf (Txdata, "Hello world index = %d\r\n", num++);
        // vTaskDelay(2000 / portTICK_PERIOD_MS);
        int ret = DHT_ReadData();
		DHT_ErrorHandler(ret);

        snprintf(data_hum, 10, "Hum %.1f\n", DHT_GetHumidity()); // puts string into buffer
        snprintf(data_temp, 12, "Tmp %.1f\r\n", DHT_GetTemperature()); // puts string into buffer
        strcat(( char *)data_humTemp, ( char *)data_hum);
        strcat(( char *)data_humTemp, ( char *)data_temp);
		// -- wait at least 2 sec before reading again ------------
		// The interval of whole process must be beyond 2 seconds !! 
		vTaskDelay( 2000 / portTICK_PERIOD_MS );
    }
}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
	//xTaskCreate(DHT_task, "DHT_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL );
}
