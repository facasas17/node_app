/* 

	DHT22 temperature sensor driver

*/

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
