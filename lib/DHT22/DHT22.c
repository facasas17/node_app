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

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <rom/ets_sys.h>

#include "DHT22.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static const char* TAG = "DHT";
#define MAXdhtData 5	// to complete 40 = 5*8 Bits
/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t DHT_GPIO;				// my default DHT pin = 4

float humidity = 0.;
float temperature = 0.;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* 
 * @brief Get State of GPIO after usTimeOut 
 */
static uint8_t getSignalLevel( uint8_t usTimeOut, uint8_t state );

/* 
 * @brief DHT will keep the line low for 80 us and then high for 80us 
 */
static int8_t DHT_Init(void);

/* 
 * @brief Get humidity from AllData[0] and AllData[1] 
 */
static void DHT_SeparateHumData(uint8_t *AllData);

/* 
 * @brief Get temperature from AllData[2] and AllData[3] 
 */
static void DHT_SeparateTempData(uint8_t *AllData);

/* 
 * @brief Verify if checksum is ok: sum of Data 8 bits masked out 0xFF 
 */
static int DHT_CheckSum(uint8_t *AllData);

/*******************************************************************************
 * Code - private
 ******************************************************************************/
static uint8_t getSignalLevel( uint8_t usTimeOut, uint8_t state )
{
	uint8_t uSec = 0;
	while( gpio_get_level(DHT_GPIO)==state ) 
	{
		if( uSec > usTimeOut ) 
			return -1;
		
		++uSec;
		ets_delay_us(1);		// uSec blocking delay
	}
	return uSec;
}

static int8_t DHT_Init(void)
{
	int time;

	/* Send start signal to DHT sensor */
	gpio_set_direction( DHT_GPIO, GPIO_MODE_OUTPUT );

	/* Pull down for 3ms for wake up */
	gpio_set_level( DHT_GPIO, 0 );
	ets_delay_us( 3000 );				// uSec blocking delay			

	/* Pull up for 25us for data */
	gpio_set_level( DHT_GPIO, 1 );
	ets_delay_us( 25 );					// uSec blocking delay

	/* Change to input mode */
	gpio_set_direction( DHT_GPIO, GPIO_MODE_INPUT );

	/* 80us Down */
	time = getSignalLevel( 85, 0 );
//	ESP_LOGI( TAG, "Response = %d", time );
	if( time<0 ) 
		return DHT_TIMEOUT_ERROR; 

	/* 80us Up */
	time = getSignalLevel( 85, 1 );
//	ESP_LOGI( TAG, "Response = %d", time );
	if( time<0 ) 
		return DHT_TIMEOUT_ERROR;
	
	return DHT_OK;
}

static void DHT_SeparateHumData(uint8_t *AllData)
{
	humidity = AllData[0];
	humidity *= 0x100;					// >> 8
	humidity += AllData[1];
	humidity /= 10;						// get the decimal
}

static void DHT_SeparateTempData(uint8_t *AllData)
{
	temperature = AllData[2] & 0x7F;	
	temperature *= 0x100;				// >> 8
	temperature += AllData[3];
	temperature /= 10;

	if( AllData[2] & 0x80 ) 			// negative temp, brrr it's freezing
		temperature *= -1;
}

static int DHT_CheckSum(uint8_t *AllData)
{
	if (AllData[4] == ((AllData[0] + AllData[1] + AllData[2] + AllData[3]) & 0xFF)) 
		return DHT_OK;
	else 
		return DHT_CHECKSUM_ERROR;
}

/*******************************************************************************
 * Code - public
 ******************************************************************************/
/*----------------------------------------------------------------------------
;
;	read DHT22 sensor

DATA (40bits): 16bits Hum + 16bits Temp + 8bits CheckSum

Example: MCU has received 40 bits data from AM2302 as
0000 0010 1000 1100 0000 0001 0101 1111 1110 1110
16 bits RH data + 16 bits T data + check sum

1) we convert 16 bits RH data from binary system to decimal system, 0000 0010 1000 1100 → 652
Binary system Decimal system: RH=652/10=65.2%RH

2) we convert 16 bits T data from binary system to decimal system, 0000 0001 0101 1111 → 351
Binary system Decimal system: T=351/10=35.1°C

When highest bit of temperature is 1, it means the temperature is below 0 degree Celsius. 
Example: 1000 0000 0110 0101, T= minus 10.1°C: 16 bits T data

3) Check Sum=0000 0010+1000 1100+0000 0001+0101 1111=1110 1110 Check-sum=the last 8 bits of Sum=11101110

Signal & Timings:

The interval of whole process must be beyond 2 seconds.

To request data from DHT:

1) Sent low pulse for > 1~10 ms
2) Sent high pulse for > 20~40 us
3) When DHT detects the start signal, it will pull low the bus 80us as response signal, 
   then the DHT pulls up 80us for preparation to send data.
4) When DHT is sending data to MCU, every bit's transmission begin with low-voltage-level that last 50us, 
   the following high-voltage-level signal's length decide the bit is "1" or "0".
	0: 26~28 us
	1: 70 us
;----------------------------------------------------------------------------*/
int DHT_ReadData()
{
	int uSec = 0;

	uint8_t dhtData[MAXdhtData];
	uint8_t byteInx = 0;
	uint8_t bitInx = 7;

	for (uint8_t k = 0; k<MAXdhtData; k++) 
		dhtData[k] = 0;

	if( DHT_Init() == DHT_TIMEOUT_ERROR)
		return DHT_TIMEOUT_ERROR;
  
  	/* No errors, read the 40 data bits */  
	for( int k = 0; k < 40; k++ ) 
	{
		/* Start new data transmision with >50us low signal */
		uSec = getSignalLevel( 56, 0 );
		if( uSec<0 ) 
			return DHT_TIMEOUT_ERROR;

		/* Check to see if after >70us RX data is 0 or 1 */
		uSec = getSignalLevel( 75, 1 );
		if( uSec<0 ) 
			return DHT_TIMEOUT_ERROR;

		/* Add the current read to the output data. 
			Only look for '1' (>28us) because array is set to '0' */
		if (uSec > 40) 
		{
			dhtData[ byteInx ] |= (1 << bitInx);
		}

		/* Index to next byte */
		if (bitInx == 0) 
		{ 
			bitInx = 7; 
			++byteInx;
		}
		else 
			bitInx--;
	}

	DHT_SeparateHumData(dhtData);
	DHT_SeparateTempData(dhtData);

	return DHT_CheckSum(dhtData);
}

float DHT_GetHumidity(void) 
{ 
	return humidity; 
}

float DHT_GetTemperature(void) 
{ 
	return temperature; 
}

void DHT_SetGpio(uint8_t gpio)
{
	DHT_GPIO = gpio;
}

void DHT_ErrorHandler(int response)
{
	switch(response) 
	{
		case DHT_TIMEOUT_ERROR :
			ESP_LOGE( TAG, "Sensor Timeout\n" );
			break;

		case DHT_CHECKSUM_ERROR:
			ESP_LOGE( TAG, "CheckSum error\n" );
			break;

		case DHT_OK:
			break;

		default :
			ESP_LOGE( TAG, "Unknown error\n" );
	}
}