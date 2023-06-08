/*
 * Copyright (c) 2023, Fabiola de las Casas Escardo <fabioladelascasas@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Version: 1.0
 * 
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*	Prioridades de las tareas */
#define DHT_TASK_PRIORITY  				(configMAX_PRIORITIES-5)
#define UART_TASK_PRIORITY  			(configMAX_PRIORITIES-2)

/* Stack de las tareas */
#define DHT_TASK_STACK 				1024*2
#define UART_TASK_STACK				1024*2

/*******************************************************************************
 * API
 ******************************************************************************/

#endif /* MAIN_MAIN_H_ */
