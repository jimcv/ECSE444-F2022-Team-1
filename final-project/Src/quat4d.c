/*
 * quat4d.c
 *
 * Implement operations related to quaternions.
 * Define quaternion as float quat4d[4] = {w, x, y, z}
 *
 *  Created on: Nov 13, 2022
 *      Author: Jimmy Sheng
 *      Formatting: BSD
 */

// private includes
#include "quat4d.h"
#include <math.h>

/**
 * The conjugate of a quaternion is defined as (w, -x, -y, -z).
 * If q is a rotation quaternion (i.e. norm(q) = 1), then its conjugate is equal to its inverse.
 *
 * *q: input quaternion
 * *result: conjugate of *q
 */
void quat_conjugate(const float *q, float *result)
{
	result[0] = q[0];
	result[1] = -q[1];
	result[2] = -q[2];
	result[3] = -q[3];
}

/**
 * *q: a rotation quaternion (i.e. normalized)
 * *eulerAngles: {x_roll, y_pitch, z_yaw}
 * useRadian: use degree if 0, otherwise use radian
 */
void quat_to_euler(const float *q, float *eulerAngles, int useRadian)
{
	// roll
	float t0 = 2.0 * (q[0] * q[1] + q[2] * q[3]);
	float t1 = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
	float x_roll = atan2f(t0, t1);
	// pitch
	float t2 = 2.0 * (q[0] * q[2] - q[3] * q[1]);
	if (t2 > 1.0)
		t2 = 1.0;
	if (t2 < -1.0)
		t2 = -1.0;
	float y_pitch = asinf(t2);
	// yaw
	float t3 = 2.0 * (q[0] * q[3] + q[1] * q[2]);
	float t4 = 1.0 - 2.0 * (q[2] * q[2] + q[3] * q[3]);
	float z_yaw = atan2f(t3, t4);

	if (useRadian)
	{
		eulerAngles[0] = x_roll;
		eulerAngles[1] = y_pitch;
		eulerAngles[2] = z_yaw;
	}
	else
	{
		eulerAngles[0] = to_degree(x_roll);
		eulerAngles[1] = to_degree(y_pitch);
		eulerAngles[2] = to_degree(z_yaw);
	}
}

/**
 * The inverse of quaternion q is defined as conjugate(q) / norm(q)^2
 *
 * *q: input quaternion
 * *result: the inverse of *q
 */
void quat_inverse(const float *q, float *result)
{
	float n2 = quat_norm_squared(q);
	float conj[4] = IDENTITY_QUATERNION;
	quat_conjugate(q, conj);
	result[0] = conj[0] / n2;
	result[1] = conj[1] / n2;
	result[2] = conj[2] / n2;
	result[3] = conj[3] / n2;
}

/**
 * Perform quaternion multiplication a * b
 *
 * *qa: first quaternion
 * *qb: second quaternion
 * :result: resulting quaternion
 */
void quat_mult(const float *qa, const float *qb, float *result)
{
	result[0] = qa[0] * qb[0] - qa[1] * qb[1] - qa[2] * qb[2] - qa[3] * qb[3];
	result[1] = qa[0] * qb[1] + qa[1] * qb[0] - qa[2] * qb[3] + qa[3] * qb[2];
	result[2] = qa[0] * qb[2] + qa[1] * qb[3] + qa[2] * qb[0] - qa[3] * qb[1];
	result[3] = qa[0] * qb[3] - qa[1] * qb[2] + qa[2] * qb[1] + qa[3] * qb[0];
}

/**
 * Return the norm (magnitude) of *q
 *
 * *q: input quaternion
 */
float quat_norm(const float *q)
{
	return sqrtf(quat_norm_squared(q));
}

/**
 * Return the norm squared of *q
 *
 * *q: input quaternion
 */
float quat_norm_squared(const float *q)
{
	return q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
}

/**
 * Perform quaternion normalization.
 *
 * *q: input quaternion
 * *result: *q normalized
 */
void quat_normalize(const float *q, float *result)
{
	float norm = quat_norm(q);
	result[0] = q[0] / norm;
	result[1] = q[1] / norm;
	result[2] = q[2] / norm;
	result[3] = q[3] / norm;
}

float to_degree(const float radian)
{
	return radian * ONE_RADIAN;
}

float to_radian(const float degree)
{
	return degree / ONE_RADIAN;
}

