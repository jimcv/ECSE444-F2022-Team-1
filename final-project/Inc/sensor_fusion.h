/*
 * sensor_fusion.h
 *
 *  Created on: Nov 13, 2022
 *      Author: Jimmy Sheng
 */

#ifndef INC_SENSOR_FUSION_H_
#define INC_SENSOR_FUSION_H_

// includes
#include "stm32l4s5i_iot01.h"
#include "stm32l4xx_hal.h"
#include "MadgwickAHRS.h"
#include "quat4d.h"
#include "main.h"

// exported defines
#define ARRAY_LEN(a) sizeof(a) / sizeof(a[0])

// exported constants
extern const IRQn_Type SENSOR_TIM_IRQ;

// exported functions
void initInput(bool reconfigurationRequested);
void fusion_calibrate_gyro();
void fusion_init(bool doCalibration, int useMagnetometer);
void fusion_update();
void fusion_get_euler(float *pEulerData, int useRadian);

#endif /* INC_SENSOR_FUSION_H_ */
