/*------------------------------------------------------------------------------

	DHT22 temperature & humidity sensor AM2302 (DHT22) driver for ESP32

	Oct 2022: Fabiola de las Casas
	Jun 2017: Ricardo Timmermann, new for DHT22  	

	Code Based on Adafruit Industries and Sam Johnston and Coffe & Beer. Please help
	to improve this code. 
	
	This example code is in the Public Domain (or CC0 licensed, at your option.)

	Unless required by applicable law or agreed to in writing, this
	software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
	CONDITIONS OF ANY KIND, either express or implied.

	PLEASE KEEP THIS CODE IN LESS THAN 0XFF LINES. EACH LINE MAY CONTAIN ONE BUG !!!

---------------------------------------------------------------------------------*/

#ifndef DHT22_H_  
#define DHT22_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DHT_OK 				0
#define DHT_CHECKSUM_ERROR 	-1
#define DHT_TIMEOUT_ERROR 	-2

#define DHT22_PIN       	GPIO_NUM_4  
/*******************************************************************************
 * API
 ******************************************************************************/
/*
 * @brief Initialize communication and request for data to DHT sensor
 */
int DHT_ReadData(void);

/*
 * @brief Return Humidity measured in float
 */
float DHT_GetHumidity(void);

/*
 * @brief Return Temperature measured in float
 */
float DHT_GetTemperature(void);

/* 
 * @brief Set DHT GPIO 
 */
void DHT_SetGpio(uint8_t gpio);

/*
 *	@brief Error Handler for timeout of data
*/
void DHT_ErrorHandler(int response);

#endif
