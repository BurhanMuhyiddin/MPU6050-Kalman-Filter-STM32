/*
 * kalmanFilter.h
 *
 *  Created on: Jul 6, 2021
 *      Author: burhan
 */

#ifndef SRC_KALMANFILTER_H_
#define SRC_KALMANFILTER_H_

#include "stm32f4xx_hal.h"

float m_xHatBar[2];
float m_pBar[2][2];
float m_k[2];
float m_xHat[2];
float m_p[2][2];
float m_q[2][2];
float m_r;

void KalmanFilter_initialize(float xHat[2], float p[2][2], float q[2][2], float r);
void KalmanFilter_update(float angularVel, float angle, float t);
float KalmanFilter_getAngle();

#endif /* SRC_KALMANFILTER_H_ */
