/*
 * kalmanFilter.c
 *
 *  Created on: Jul 6, 2021
 *      Author: burhan
 */

#include "kalmanFilter.h"

void KalmanFilter_initialize(Kalman_ParamsTypeDef* params, float xHat[2], float p[2][2], float q[2][2], float r)
{
	params->m_r = r;

	for (int i = 0; i < 2; i++)
	{
		params->m_xHat[i] = xHat[i];
		params->m_xHatBar[i] = 0.0;
		params->m_k[i] = 0.0;
		for (int j = 0; j < 2; j++)
		{
			params->m_pBar[i][j] = 0.0;
			params->m_p[i][j] = p[i][j];
			params->m_q[i][j] = q[i][j];
		}
	}
}

void KalmanFilter_update(Kalman_ParamsTypeDef* params, float angularVel, float angle, float t)
{
    // Prediction
	params->m_xHatBar[0] = params->m_xHat[0] + t * (angularVel - params->m_xHat[1]);
	params->m_xHatBar[1] = params->m_xHat[1];

	params->m_pBar[0][0] = params->m_p[0][0] + t * ((t * params->m_p[1][1]) - params->m_p[1][0] - params->m_p[0][1] + params->m_q[0][0]);
	params->m_pBar[0][1] = params->m_p[0][1] + t * (params->m_q[0][1] - params->m_p[1][1]);
	params->m_pBar[1][0] = params->m_p[1][0] + t * (params->m_q[1][0] - params->m_p[1][1]);
	params->m_pBar[1][1] = params->m_p[1][1] + t * (params->m_q[1][1]);

    // Update
	params->m_k[0] = params->m_pBar[0][0] / (params->m_pBar[0][0] + params->m_r);
	params->m_k[1] = params->m_pBar[1][0] / (params->m_pBar[0][0] + params->m_r);

	params->m_xHat[0] = params->m_xHatBar[0] + params->m_k[0] * (angle - params->m_xHatBar[0]);
    params->m_xHat[1] = params->m_xHatBar[1] + params->m_k[1] * (angle - params->m_xHatBar[0]);

    params->m_p[0][0] = params->m_pBar[0][0] - (params->m_k[0] * params->m_pBar[0][0]);
    params->m_p[0][1] = params->m_pBar[0][1] - (params->m_k[0] * params->m_pBar[0][1]);
    params->m_p[1][0] = params->m_pBar[1][0] - (params->m_k[1] * params->m_pBar[0][0]);
    params->m_p[1][1] = params->m_pBar[1][1] - (params->m_k[1] * params->m_pBar[0][1]);
}

float KalmanFilter_getAngle(Kalman_ParamsTypeDef* params)
{
	return params->m_xHat[0];
}
