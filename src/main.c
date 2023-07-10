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
// #include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if NODE_TEST
#include "driver/gpio.h"
#include "driver/uart.h"
#endif

#include "DHT22.h"
#include "uartDriver.h"
#include "protocol.h"

#include "esp_mac.h"
#include "crc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_BAUDRATE   115200
#define UART_PORT       UART_NUM_0

#define BUF_SIZE        1024
#define SIZE_MAC        6
#define TASK_DELAY      500

#if NODE_TEST
#define RE_DE_CP2102PIN GPIO_NUM_5
#define RS485_RECIEVE   0
#define RS485_TRANSMIT  1
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hardware_config(void);
static void mainTask(void *arg);

/*******************************************************************************
 * Variables
 ******************************************************************************/
char data_buffer[BUF_SIZE];
uint8_t node_address;

QueueHandle_t uart_queue;

uint8_t prot_action = 0x02;
uint8_t prot_flag = 0x02;

/*******************************************************************************
 * Code - private
 ******************************************************************************/
static void hardware_config(void)
{
    uint8_t base_mac_addr[SIZE_MAC] = {0};

    esp_read_mac(base_mac_addr, ESP_MAC_WIFI_STA);
    node_address = crc_calc(0, base_mac_addr, SIZE_MAC);

    UART_Config(UART_PORT, UART_BAUDRATE, BUF_SIZE, &uart_queue, node_address);
    DHT_SetGpio();
    
    RS485_ConfigGPIO();
}

static void mainTask(void *arg)
{
    int dht_error;
    uint16_t len_data = 0;
    uart_event_t event;
    protocol_frame_t frame;
 
    hardware_config();
    memset(data_buffer, 0, BUF_SIZE);      // Reset buffer

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
                dht_error = DHT_ReadData();
                DHT_ErrorHandler(dht_error);

                protocol_setFrame(&frame, node_address, prot_action, prot_flag, DHT_GetTemperature(), DHT_GetHumidity()); 

                len_data = protocol_buildFrame(data_buffer, &frame);

                RS485_EnableSendData();

        #if NODE_TEST
                gpio_set_level( RE_DE_CP2102PIN, RS485_RECIEVE );  
        #endif

                if( UART_OK == UART_WaitTX(UART_PORT))
                {
                    UART_SendData(UART_PORT, data_buffer, len_data);
                }
            }
            else
            {

        #if NODE_TEST
                gpio_set_level( RE_DE_PIN, RS485_TRANSMIT );       
                gpio_set_level( RE_DE_CP2102PIN, RS485_RECIEVE );  
                UART_SendData(UART_PORT, "\nSin pedido de data\n", strlen("\nSin pedido de data\n"));
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

