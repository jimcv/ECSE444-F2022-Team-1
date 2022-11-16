/*
 * quat4d.h
 *
 *  Created on: Nov 13, 2022
 *      Author: Jimmy Sheng
 *      Formatting: BSD
 */

#ifndef INC_QUAT4D_H_
#define INC_QUAT4D_H_

// constants
#define IDENTITY_QUATERNION {1.0, 0.0, 0.0, 0.0}
#define ONE_RADIAN 57.295777951

// export functions
void quat_conjugate(const float *q, float *result);
void quat_to_euler(const float *q, float *eulerAngles, int useRadian);
void quat_inverse(const float *q, float *result);
void quat_mult(const float *q1, const float *q2, float *result);
float quat_norm(const float *q);
float quat_norm_squared(const float *q);
void quat_normalize(const float *q, float *result);
float to_degree(const float radian);
float to_radian(const float degree);

#endif /* INC_QUAT4D_H_ */
