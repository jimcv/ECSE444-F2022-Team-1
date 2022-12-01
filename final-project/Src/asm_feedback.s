/*
 * asm_feedback.s
 *
 *  Created on: Dec 1, 2022
 *      Author: Jimmy Sheng
 */

.syntax unified
.global asm_feedback
.section .text.rodata

HALF:	.word	0x3f000000	// 0.5f

/*
void asm_feedback(float *pIn, float *pOut);
In[11] = {ax, ay, az, gx, gy, gz, q0, q1, q2, q3, beta};
Out[4] = {qDot1, qDot2, qDot3, qDot4};
*/
asm_feedback:
	PUSH			{LR}
	VLDM.F32		R0, {S1-S11}
	// normalize accelerometer
	VMUL.F32		S0, S1, S1
	VMLA.F32		S0, S2, S2
	VMLA.F32		S0, S3, S3
	BL				asm_inv_sqrt
	VMUL.F32		S1, S1, S0
	VMUL.F32		S2, S2, S0
	VMUL.F32		S3, S3, S0
	// compute temporary variables
	VMOV.F32		S0, #2
	VMUL.F32		S12, S0, S7			// S12 <- _2q0
	VMUL.F32		S13, S0, S8			// S13 <- _2q1
	VMUL.F32		S14, S0, S9			// S14 <- _2q2
	VMUL.F32		S15, S0, S10		// S15 <- _2q3
	VMOV.F32		S0, #4
	VMUL.F32		S16, S0, S7			// S16 <- _4q0
	VMUL.F32		S17, S0, S8			// S17 <- _4q1
	VMUL.F32		S18, S0, S9			// S18 <- _4q2
	VMOV.F32		S0, #8
	VMUL.F32		S19, S0, S8			// S19 <- _8q1
	VMUL.F32		S20, S0, S9			// S20 <- _8q2
	VMUL.F32		S21, S7, S7			// S21 <- q0q0
	VMUL.F32		S22, S8, S8			// S22 <- q1q1
	VMUL.F32		S23, S9, S9			// S23 <- q2q2
	VMUL.F32		S24, S10, S10		// S24 <- q3q3
	// gradient descent corrective step
	VMUL.F32		S25, S16, S23		// S25 <- s0
	VMLA.F32		S25, S14, S1
	VMLA.F32		S25, S16, S22
	VMLS.F32		S25, S13, S2
	VMUL.F32		S26, S17, S24		// S26 <- s1
	VMLS.F32		S26, S15, S1		// - _2q3 * ax
	VMOV.F32		S0, #4
	VMUL.F32		S0, S0, S21
	VMLA.F32		S26, S0, S8			// + 4.0f * q0q0 * q1
	VMLS.F32		S26, S12, S2		// - _2q0 * ay
	VSUB.F32		S26, S26, S17		// - _4q1
	VMLA.F32		S26, S19, S22		// + _8q1 * q1q1
	VMLA.F32		S26, S19, S23		// + _8q1 * q2q2
	VMLA.F32		S26, S17, S3		// + _4q1 * az
	VMOV.F32		S27, #4
	VMUL.F32		S27, S27, S21
	VMUL.F32		S27, S27, S9		// S27 <- s2
	VMLA.F32		S27, S12, S1		// + _2q0 * ax
	VMLA.F32		S27, S18, S24		// + _4q2 * q3q3
	VMLS.F32		S27, S15, S2		// - _2q3 * ay
	VSUB.F32		S27, S27, S18		// - _4q2
	VMLA.F32		S27, S20, S22		// + _8q2 * q1q1
	VMLA.F32		S27, S20, S23		// + _8q2 * q2q2
	VMLA.F32		S27, S18, S3		// + _4q2 * az
	VMOV.F32		S28, #4
	VMUL.F32		S28, S28, S22
	VMUL.F32		S28, S28, S10		// S28 <- s3
	VMLS.F32		S28, S13, S1		// - _2q1 * ax
	VMOV.F32		S0, #4
	VMUL.F32		S0, S0, S23
	VMLA.F32		S28, S0, S10		// + 4.0f * q2q2 * q3
	VMLS.F32		S28, S14, S2		// - _2q2 * ay
	// normalize s
	VMUL.F32		S0, S25, S25
	VMLA.F32		S0, S26, S26
	VMLA.F32		S0, S27, S27
	VMLA.F32		S0, S28, S28
	BL				asm_inv_sqrt
	VMUL.F32		S25, S25, S0
	VMUL.F32		S26, S26, S0
	VMUL.F32		S27, S27, S0
	VMUL.F32		S28, S28, S0
	// apply feedback step
	VLDM.F32		R1, {S12-S15}
	VMLS.F32		S12, S11, S25		// qDot1 -= beta * s0
	VMLS.F32		S13, S11, S26		// qDot2 -= beta * s1
	VMLS.F32		S14, S11, S27		// qDot3 -= beta * s2
	VMLS.F32		S15, S11, S28		// qDot4 -= beta * s3
	VSTM.F32		R1, {S12-S15}
	POP				{LR}
	BX				LR

/*
Compute inverse square root of S0
Return in S0
*/
asm_inv_sqrt:
	VPUSH.F32		{S1-S2}
	VSQRT.F32		S1, S0
	VMOV.F32		S2, #1
	VDIV.F32		S0, S2, S1
	VPOP.F32		{S1-S2}
	BX				LR
