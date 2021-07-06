/*
 * mpu6050.c
 *
 *  Created on: Jun 22, 2021
 *      Author: burhan
 */

#include "mpu6050.h"

static I2C_HandleTypeDef i2cHandler;
static float accelScalingFactor, gyroScalingFactor;
static float A_X_Bias = 0.0f;
static float A_Y_Bias = 0.0f;
static float A_Z_Bias = 0.0f;

static int16_t GyroRW[3];

void MPU6050_Init(I2C_HandleTypeDef *I2Chnd)
{
	memcpy(&i2cHandler, I2Chnd, sizeof(*I2Chnd));
}

void I2C_Read(uint8_t ADDR, uint8_t *i2cBuf, uint8_t NofData)
{
	uint8_t i2cBuf_[2];
	uint8_t MPUADDR;

	MPUADDR = (MPU_ADDR << 1);
	i2cBuf_[0] = ADDR;
	HAL_I2C_Master_Transmit(&i2cHandler, MPUADDR, i2cBuf_, 1, 10);
	HAL_I2C_Master_Receive(&i2cHandler, MPUADDR, i2cBuf, NofData, 100);
}

void I2C_Write8(uint8_t ADDR, uint8_t data)
{
	uint8_t i2cBuf[2];
	uint8_t MPUADDR;

	i2cBuf[0] = ADDR;
	i2cBuf[1] = data;

	MPUADDR = (MPU_ADDR << 1);
	HAL_I2C_Master_Transmit(&i2cHandler, MPUADDR, i2cBuf, 2, 100);
}

void MPU6050_Config(MPU_ConfigTypeDef *config)
{
	uint8_t Buffer = 0;

	// Reset Device and disable temp sensor
	I2C_Write8(PWR_MGMT_1, 0x80);
	HAL_Delay(100);

	Buffer = config->ClockSource & 0x07;
	Buffer |= (config->Sleep_Mode_Bit << 6) & 0x04;
	I2C_Write8(PWR_MGMT_1, Buffer);
	HAL_Delay(100);

	// Set DLPF
	Buffer = 0;
	Buffer = config->CONFIG_DLPF & 0x07;
	I2C_Write8(CONFIG, Buffer);

	// Select gyro full scale range
	Buffer = 0;
	Buffer = (config->Gyro_Full_Scale << 3) & 0x18;
	I2C_Write8(GYRO_CONFIG, Buffer);

	// Select accel full scale range
	Buffer = 0;
	Buffer = (config->Accel_Full_Scale << 3) & 0x18;
	I2C_Write8(ACCEL_CONFIG, Buffer);

	MPU6050_Set_SMPRT_DIV(0x04);

	// Set accel and gyro scaling factors
	switch (config->Accel_Full_Scale) {
		case AFS_SEL_2g:
			accelScalingFactor = (2000.0f / 32768.0f);
			break;
		case AFS_SEL_4g:
			accelScalingFactor = (4000.0f / 32768.0f);
			break;
		case AFS_SEL_8g:
			accelScalingFactor = (8000.0f / 32768.0f);
			break;
		case AFS_SEL_16g:
			accelScalingFactor = (16000.0f / 32768.0f);
			break;
		default:
			break;
	}

	switch (config->Accel_Full_Scale) {
		case FS_SEL_250:
			gyroScalingFactor = (250.0f / 32768.0f);
			break;
		case FS_SEL_500:
			gyroScalingFactor = (500.0f / 32768.0f);
			break;
		case FS_SEL_1000:
			gyroScalingFactor = (1000.0f / 32768.0f);
			break;
		case FS_SEL_2000:
			gyroScalingFactor = (2000.0f / 32768.0f);
			break;
		default:
			break;
	}
}

uint8_t MPU6050_Get_SMPRT_DIV(void)
{
	uint8_t Buffer = 0;

	I2C_Read(SMPLRT_DIV, &Buffer, 1);
	return Buffer;
}

void MPU6050_Set_SMPRT_DIV(uint8_t SMPRTvalue)
{
	I2C_Write8(SMPLRT_DIV, SMPRTvalue);
}

void MPU6050_Get_Accel_RawData(RawData_Def *rawDef)
{
	uint8_t i2cBuf[2];
	uint8_t AccArr[6], GyroArr[6];

	I2C_Read(INT_STATUS, &i2cBuf[1], 1);
	if ((i2cBuf[1] & 0x01))
	{
		I2C_Read(ACCEL_XOUT_H, AccArr, 6);

		rawDef->x = ( AccArr[0] << 8 ) + AccArr[1];
		rawDef->y = ( AccArr[2] << 8 ) + AccArr[3];
		rawDef->z = ( AccArr[4] << 8 ) + AccArr[5];

		I2C_Read(GYRO_XOUT_H, GyroArr, 6);
		GyroRW[0] = ( GyroArr[0] << 8 ) + GyroArr[1];
		GyroRW[1] = ( GyroArr[2] << 8 ) + GyroArr[3];
		GyroRW[2] = ( GyroArr[4] << 8 ) + GyroArr[5];
	}
}

void MPU6050_Get_Accel_ScaledData(ScaledData_Def *scaledDef)
{
	RawData_Def AccRData;

	MPU6050_Get_Accel_RawData(&AccRData);

	scaledDef->x = (AccRData.x + 0.0f) * accelScalingFactor;
	scaledDef->y = (AccRData.y + 0.0f) * accelScalingFactor;
	scaledDef->z = (AccRData.z + 0.0f) * accelScalingFactor;
}

void MPU6050_Get_Accel_Cali(ScaledData_Def *CaliDef)
{
	ScaledData_Def accSData;

	MPU6050_Get_Accel_ScaledData(&accSData);

	CaliDef->x = accSData.x - A_X_Bias;
	CaliDef->y = accSData.y - A_Y_Bias;
	CaliDef->z = accSData.z - A_Z_Bias;
}

void MPU6050_Get_Gyro_RawData(RawData_Def *rawDef)
{
	rawDef->x = GyroRW[0];
	rawDef->y = GyroRW[1];
	rawDef->z = GyroRW[2];
}

void MPU6050_Get_Gyro_ScaledData(ScaledData_Def *scaledDef)
{
	RawData_Def gyroRData;

	MPU6050_Get_Gyro_RawData(&gyroRData);

	scaledDef->x = gyroRData.x * gyroScalingFactor;
	scaledDef->y = gyroRData.y * gyroScalingFactor;
	scaledDef->z = gyroRData.z * gyroScalingFactor;
}

void _Accel_Cali(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max)
{
	A_X_Bias = (x_max + x_min) / 2.0f;
	A_Y_Bias = (y_max + y_min) / 2.0f;
	A_Z_Bias = (z_max + z_min) / 2.0f;
}

void getTiltAngle_Accel(ScaledData_Def *acc_inp, TiltAngle_Def *acc_angle)
{
	float ax = acc_inp->x;
	float ay = acc_inp->y;
	float az = acc_inp->z;
	acc_angle->roll  = atan2( ax, sqrt(ay * ay + az * az) );
	acc_angle->pitch = atan2( ay, sqrt(ax * ax + az * az) );
	acc_angle->yaw   = atan2( sqrt(ax * ax + ay * ay), az );
}

void getTiltAngle_Gyro(ScaledData_Def *gyro_inp, float dt, TiltAngle_Def *gyr_angle)
{
	gyr_angle->roll  = gyr_angle->roll  + gyro_inp->x * dt;
	gyr_angle->pitch = gyr_angle->pitch + gyro_inp->y * dt;
	gyr_angle->yaw   = gyr_angle->yaw   + gyro_inp->z * dt;
}
