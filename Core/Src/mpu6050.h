/*
 * mpu6050.h
 *
 *  Created on: Jun 22, 2021
 *      Author: burhan
 */

#ifndef SRC_MPU6050_H_
#define SRC_MPU6050_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Define registers
#define WHO_AM_I_REG	0x75
#define MPU_ADDR		0x68
#define PWR_MGMT_1		0x6B
#define CONFIG			0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG	0x1C
#define SMPLRT_DIV		0x19
#define INT_ENABLE		0x38
#define INT_STATUS		0x3A
#define ACCEL_XOUT_H	0x3B
#define GYRO_XOUT_H		0x43

// TypeDefs and Enums
// MPU Configuration
typedef struct
{
	uint8_t ClockSource;
	uint8_t Gyro_Full_Scale;
	uint8_t Accel_Full_Scale;
	uint8_t CONFIG_DLPF;
	bool    Sleep_Mode_Bit;
} MPU_ConfigTypeDef;

//Clock Source Enum
enum PM_CLKSEL_ENUM
{
	Internal_8MHz	= 0x00,
	X_Axis_Ref		= 0x01,
	Y_Axis_Ref		= 0x02,
	Z_Axis_Ref		= 0x03,
	Ext_32_768KHz	= 0x04,
	Ext_19_2MHz		= 0x05,
	TIM_GENT_INREST	= 0x07
};

// Gyro Full Scale Enum (deg / sec)
enum gyro_FullScale_Enum
{
	FS_SEL_250	= 0x00,
	FS_SEL_500 	= 0x01,
	FS_SEL_1000 = 0x02,
	FS_SEL_2000 = 0x03
};

// Accelerometer Full Scale Enum (1g = 9.81 m/sec2)
enum accel_FullScale_ENUM
{
	AFS_SEL_2g	= 0x00,
	AFS_SEL_4g 	= 0x01,
	AFS_SEL_8g 	= 0x02,
	AFS_SEL_16g = 0x03
};

// DLPF Enum
enum DLPF_CFG_ENUM
{
	DLPF_260A_256G_Hz	= 0x00,
	DLPF_184A_188G_Hz	= 0x01,
	DLPF_94A_98G_Hz 	= 0x02,
	DLPF_44A_42G_Hz 	= 0x03,
	DLPF_21A_20G_Hz 	= 0x04,
	DLPF_10_Hz 			= 0x05,
	DLPF_5_Hz 			= 0x06
};

// External Frame Synchronization Enum
enum EXT_SYNC_SET_ENUM
{
	input_Disable = 0x00,
	TEMP_OUT_L		= 0x01,
	GYRO_XOUT_L		= 0x02,
	GYRO_YOUT_L		= 0x03,
	GYRO_ZOUT_L		= 0x04,
	ACCEL_XOUT_L	= 0x05,
	ACCEL_YOUT_L	= 0x06,
	ACCEL_ZOUT_L	= 0x07
};

// Raw data typedef
typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
} RawData_Def;

// Scaled data typedef
typedef struct
{
	float x;
	float y;
	float z;
} ScaledData_Def;

// Tilt angles
typedef struct
{
	float roll;
	float pitch;
	float yaw;
} TiltAngle_Def;

void MPU6050_Init(I2C_HandleTypeDef *I2Chnd);
void I2C_Read(uint8_t ADDR, uint8_t *i2cBuf, uint8_t NofData);
void I2C_Write8(uint8_t ADDR, uint8_t data);
void MPU6050_Config(MPU_ConfigTypeDef *config);
uint8_t MPU6050_Get_SMPRT_DIV(void);
void MPU6050_Set_SMPRT_DIV(uint8_t SMPRTvalue);
void MPU6050_Get_Accel_RawData(RawData_Def *rawDef);
void MPU6050_Get_Accel_ScaledData(ScaledData_Def *scaledDef);
void MPU6050_Get_Accel_Cali(ScaledData_Def *CaliDef);
void MPU6050_Get_Gyro_RawData(RawData_Def *rawDef);
void MPU6050_Get_Gyro_ScaledData(ScaledData_Def *scaledDef);
void _Accel_Cali(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);
void getTiltAngle_Accel(ScaledData_Def *acc_inp, TiltAngle_Def *acc_angle);
void getTiltAngle_Gyro(ScaledData_Def *gyro_inp, float dt, TiltAngle_Def *gyr_angle);

#endif /* SRC_MPU6050_H_ */
