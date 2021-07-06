/*
 * kalmanFilter.c
 *
 *  Created on: Jul 6, 2021
 *      Author: burhan
 */

#include "kalmanFilter.h"

void KalmanFilter_initialize(float xHat[2], float p[2][2], float q[2][2], float r)
{
	m_r = r;

	for (int i = 0; i < 2; i++)
	{
		m_xHat[i] = xHat[i];
		m_xHatBar[i] = 0.0;
		m_k[i] = 0.0;
		for (int j = 0; j < 2; j++)
		{
			m_pBar[i][j] = 0.0;
			m_p[i][j] = p[i][j];
			m_q[i][j] = q[i][j];
		}
	}
}

void KalmanFilter_update(float angularVel, float angle, float t)
{
    // Prediction
    m_xHatBar[0] = m_xHat[0] + t * (angularVel - m_xHat[1]);
    m_xHatBar[1] = m_xHat[1];

    m_pBar[0][0] = m_p[0][0] + t * ((t * m_p[1][1]) - m_p[1][0] - m_p[0][1] + m_q[0][0]);
    m_pBar[0][1] = m_p[0][1] + t * (m_q[0][1] - m_p[1][1]);
    m_pBar[1][0] = m_p[1][0] + t * (m_q[1][0] - m_p[1][1]);
    m_pBar[1][1] = m_p[1][1] + t * (m_q[1][1]);

    // Update
    m_k[0] = m_pBar[0][0] / (m_pBar[0][0] + m_r);
    m_k[1] = m_pBar[1][0] / (m_pBar[0][0] + m_r);

    m_xHat[0] = m_xHatBar[0] + m_k[0] * (angle - m_xHatBar[0]);
    m_xHat[1] = m_xHatBar[1] + m_k[1] * (angle - m_xHatBar[0]);

    m_p[0][0] = m_pBar[0][0] - (m_k[0] * m_pBar[0][0]);
    m_p[0][1] = m_pBar[0][1] - (m_k[0] * m_pBar[0][1]);
    m_p[1][0] = m_pBar[1][0] - (m_k[1] * m_pBar[0][0]);
    m_p[1][1] = m_pBar[1][1] - (m_k[1] * m_pBar[0][1]);
}

float KalmanFilter_getAngle()
{
	return m_xHat[0];
}
