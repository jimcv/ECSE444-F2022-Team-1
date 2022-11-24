/*
 * sensor_fusion.c
 *
 *  Created on: Nov 13, 2022
 *      Author: Jimmy Sheng
 */

// sensor config
#define ACCEL_CTRL		0x4C	// 104 Hz, 8 G full-scale
#define GYRO_CTRL		0x4C	// 104 Hz, 2000 dps full-scale
#define LSM6_COMMON		0x04	// continuous update, auto increment
#define MAG_CTRL1		0x72	// 155 Hz, XY axes ultra high performance mode, FAST_ODR
#define MAG_CTRL2		0x00	// 4 gauss full-scale
#define MAG_CTRL3		0x00	// continuous conversion mode
#define MAG_CTRL4		0x0C	// Z axis ultra high performance mode
#define MAG_CTRL5		0x00	// continuous update

// private constants (for 8 G, 2000 dps, 4 gauss, full-scale)
#define LA_SO	0.000244	// LSM6 linear acceleration sensitivity in g/LSB (datasheet p.15)
#define G_SO	0.07		// LSM6 angular rate sensitivity in dps/LSB (datasheet p.15)
#define GN		6842.0		// LIS3MDL sensitivity in LSB/gauss (datasheet p.8)
#define G		9.80665		// Earth's gravity in m/s^2

// private includes
#include "sensor_fusion.h"
#include "flash_config.h"
#include "../Components/lsm6dsl/lsm6dsl.h"
#include "../Components/lis3mdl/lis3mdl.h"

// exported constants
const IRQn_Type SENSOR_TIM_IRQ = TIM3_IRQn;

// private variables
input_configuration inputConfig;

static int16_t mag_hard_iron[3] = {0, 0, 0};
static int16_t accel[3] = {0, 0, 0};
static int16_t gyro[3] = {0, 0, 0};
static int16_t mag[3] = {0, 0, 0};
static float a[3] = {0.0, 0.0, 0.0};
static float g[3] = {0.0, 0.0, 0.0};
static float m[3] = {0.0, 0.0, 0.0};
static int useMag = 0;

// private functions
static void LSM6_init();
static void LSM6_read(int16_t *pAccelData, int16_t *pGyroData);
static void LSM6_read_accel(int16_t *pAccelData);
static void LSM6_read_gyro(int16_t *pGyroData);
static void LIS3MDL_init();
static void LIS3MDL_read(int16_t *pMagData);
static void accel_to_g(int16_t *pData, float *pResult);
static void accel_to_ms2(int16_t *pData, float *pResult);
static void gyro_to_dps(int16_t *pData, float *pResult);
static void gyro_to_radps(int16_t *pData, float *pResult);
static void mag_to_gauss(int16_t *pData, float *pResult);
static void mag_to_ut(int16_t *pData, float *pResult);

/**
 * Initialize input configuration.
 * @param reconfigurationRequested whether to manually write configuration.
 */
void initInput(bool reconfigurationRequested)
{
  if (reconfigurationRequested)
  {
    // delay to prevent button bounce that affects sensor calibration
    HAL_Delay(500);

    fusion_init(true, 0);

    setInputConfiguration(&inputConfig);
  }
  else
  {
    getInputConfiguration(&inputConfig);
    fusion_init(false, 0);
  }
}

/**
 * Put the board down on a surface to capture the gyro offset.
 */
void fusion_calibrate_gyro()
{
	// disable IRQ if enabled
	uint32_t irq_en = __NVIC_GetEnableIRQ(SENSOR_TIM_IRQ);
	if (irq_en)
	{
		__NVIC_DisableIRQ(SENSOR_TIM_IRQ);
	}
	const uint16_t num_samples = 10; // should be less than 65535 to not overflow
	int16_t gyro_x[num_samples];
	int16_t gyro_y[num_samples];
	int16_t gyro_z[num_samples];
	int16_t buffer[3];
	// measure
	for (uint16_t i = 0; i < num_samples; i++)
	{
		HAL_Delay(10); // approx. 100 Hz
		LSM6_read_gyro(buffer);
		gyro_x[i] = buffer[0];
		gyro_y[i] = buffer[1];
		gyro_z[i] = buffer[2];
	}
	// calculate mean
	int32_t sum[3] = {0, 0, 0};
	for (uint16_t i = 0; i < num_samples; i++)
	{
		sum[0] += gyro_x[i];
		sum[1] += gyro_y[i];
		sum[2] += gyro_z[i];
	}
	inputConfig.gyro_offset[0] = sum[0] / num_samples;
	inputConfig.gyro_offset[1] = sum[1] / num_samples;
	inputConfig.gyro_offset[2] = sum[2] / num_samples;
	// restore IRQ
	if (irq_en)
	{
		__NVIC_EnableIRQ(SENSOR_TIM_IRQ);
	}
}

/**
 * Initialize sensors and Madgwick algorithm. The board should not be moved during the execution of this function.
 *
 * doCalibrate: if the board needs to be calibrated.
 * useMagnetometer: if magnetometer is used, Z axis drift will be corrected, but there will be magnetic disturbances.
 * 					if 0, magnetometer is not used and Z axis will have drift.
 */
void fusion_init(bool doCalibrate, int useMagnetometer)
{
	// disable IRQ if enabled
	uint32_t irq_en = __NVIC_GetEnableIRQ(SENSOR_TIM_IRQ);
	if (irq_en)
	{
		__NVIC_DisableIRQ(SENSOR_TIM_IRQ);
	}
	useMag = useMagnetometer;
	LSM6_init();
	LIS3MDL_init();
	if (doCalibrate)
	{
	  fusion_calibrate_gyro();
	}
	// Need to run Madgwick a little bit to make it converge to absolute Z axis value based on magnetic field
	if (useMag)
	{
		float oldBeta = beta;
		beta = 10.0f;
		for (uint32_t i = 0; i < 100; i++)
		{
			HAL_Delay(10);
			fusion_update();
		}
		beta = oldBeta;
	}
	// restore IRQ
	if (irq_en)
	{
		__NVIC_EnableIRQ(SENSOR_TIM_IRQ);
	}
}

/**
 * Run Madgwick algorithm to update the rotation quaternion.
 */
void fusion_update()
{
	// disable IRQ if enabled
	uint32_t irq_en = __NVIC_GetEnableIRQ(SENSOR_TIM_IRQ);
	if (irq_en)
	{
		__NVIC_DisableIRQ(SENSOR_TIM_IRQ);
	}
	LSM6_read(accel, gyro);
	if (useMag)
	{
		LIS3MDL_read(mag);
	}
	accel_to_ms2(accel, a);
	gyro_to_radps(gyro, g);
	if (useMag)
	{
		mag_to_ut(mag, m);
		MadgwickAHRSupdate(g[0], g[1], g[2], a[0], a[1], a[2], m[0], m[1], m[2]);
	}
	else
	{
		MadgwickAHRSupdateIMU(g[0], g[1], g[2], a[0], a[1], a[2]);
	}
	// restore IRQ
	if (irq_en)
	{
		__NVIC_EnableIRQ(SENSOR_TIM_IRQ);
	}
}

/**
 * Convert rotation quaternion to Euler angles.
 *
 * *pEulerData: Euler angles in XYZ
 * useRadian: use degree if 0, else use radian
 */
void fusion_get_euler(float *pEulerData, int useRadian)
{
	// disable IRQ if enabled
	uint32_t irq_en = __NVIC_GetEnableIRQ(SENSOR_TIM_IRQ);
	if (irq_en)
	{
		__NVIC_DisableIRQ(SENSOR_TIM_IRQ);
	}
	float quat[4];
	quat[0] = q0;
	quat[1] = q1;
	quat[2] = q2;
	quat[3] = q3;
	quat_to_euler(quat, pEulerData, useRadian);
	// restore IRQ
	if (irq_en)
	{
		__NVIC_EnableIRQ(SENSOR_TIM_IRQ);
	}
}

static void LSM6_init()
{
	uint8_t temp;
	// CTRL1_XL
	temp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL);
	temp &= ~(0xFC); // mask
	temp |= ACCEL_CTRL;
	SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL, temp);
	// CTRL2_G
	temp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G);
	temp &= ~(0xFC); // mask
	temp |= GYRO_CTRL;
	SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G, temp);
	// CTRL3_C
	temp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C);
	temp &= ~(0x44); // mask
	temp |= LSM6_COMMON;
	SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C, temp);
}

static void LSM6_read(int16_t *pAccelData, int16_t *pGyroData)
{
	LSM6_read_accel(pAccelData);
	LSM6_read_gyro(pGyroData);
}

static void LSM6_read_accel(int16_t *pAccelData)
{
	uint8_t buffer[6];
	// accel
	SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_OUTX_L_XL, buffer, 6);
	// combine high and low bytes
	pAccelData[0] = (int16_t)(buffer[1] << 8 | buffer[0]);
	pAccelData[1] = (int16_t)(buffer[3] << 8 | buffer[2]);
	pAccelData[2] = (int16_t)(buffer[5] << 8 | buffer[4]);
}

static void LSM6_read_gyro(int16_t *pGyroData)
{
	uint8_t buffer[6];
	// gyro
	SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_OUTX_L_G, buffer, 6);
	pGyroData[0] = (int16_t)(buffer[1] << 8 | buffer[0]) - inputConfig.gyro_offset[0];
	pGyroData[1] = (int16_t)(buffer[3] << 8 | buffer[2]) - inputConfig.gyro_offset[1];
	pGyroData[2] = (int16_t)(buffer[5] << 8 | buffer[4]) - inputConfig.gyro_offset[2];
}

static void LIS3MDL_init()
{
	SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG1, MAG_CTRL1);
	SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG2, MAG_CTRL2);
	SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG3, MAG_CTRL3);
	SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG4, MAG_CTRL4);
	SENSOR_IO_Write(LIS3MDL_MAG_I2C_ADDRESS_HIGH, LIS3MDL_MAG_CTRL_REG5, MAG_CTRL5);
}

static void LIS3MDL_read(int16_t *pMagData)
{
	uint8_t buffer[6];
	// assert MSB (0x80) to enable address auto-increment
	SENSOR_IO_ReadMultiple(LIS3MDL_MAG_I2C_ADDRESS_HIGH, (LIS3MDL_MAG_OUTX_L | 0x80), buffer, 6);
	pMagData[0] = (int16_t)(buffer[1] << 8 | buffer[0]) - mag_hard_iron[0];
	pMagData[1] = (int16_t)(buffer[3] << 8 | buffer[2]) - mag_hard_iron[1];
	pMagData[2] = (int16_t)(buffer[5] << 8 | buffer[4]) - mag_hard_iron[2];
}

static void accel_to_g(int16_t *pData, float *pResult)
{
	pResult[0] = pData[0] * LA_SO;
	pResult[1] = pData[1] * LA_SO;
	pResult[2] = pData[2] * LA_SO;
}

static void accel_to_ms2(int16_t *pData, float *pResult)
{
	accel_to_g(pData, pResult);
	pResult[0] *= G;
	pResult[1] *= G;
	pResult[2] *= G;
}

static void gyro_to_dps(int16_t *pData, float *pResult)
{
	pResult[0] = pData[0] * G_SO;
	pResult[1] = pData[1] * G_SO;
	pResult[2] = pData[2] * G_SO;
}

static void gyro_to_radps(int16_t *pData, float *pResult)
{
	gyro_to_dps(pData, pResult);
	pResult[0] = to_radian(pResult[0]);
	pResult[1] = to_radian(pResult[1]);
	pResult[2] = to_radian(pResult[2]);
}

static void mag_to_gauss(int16_t *pData, float *pResult)
{
	pResult[0] = pData[0] / GN;
	pResult[1] = pData[1] / GN;
	pResult[2] = pData[2] / GN;
}

static void mag_to_ut(int16_t *pData, float *pResult)
{
	mag_to_gauss(pData, pResult);
	pResult[0] *= 100.0;
	pResult[1] *= 100.0;
	pResult[2] *= 100.0;
}
