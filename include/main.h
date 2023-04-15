/*
 * Copyright 2023
 * All rights reserved.
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
#define DHT_TASK_PRIORITY  				(configMAX_PRIORITIES - 1)
#define CMD_TASK_PRIORITY  				(configMAX_PRIORITIES - 2)

/* Stack de las tareas */
#define DHT_TASK_STACK 				1024*2
#define CMD_TASK_STACK				configMINIMAL_STACK_SIZE*4

/*******************************************************************************
 * API
 ******************************************************************************/

#endif /* MAIN_MAIN_H_ */
