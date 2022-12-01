/*
 * asm_inv_sqrt.s
 *
 *  Created on: Dec 1, 2022
 *      Author: Jimmy Sheng
 */

.syntax unified			// mixed 16-bit and 32-bit instructions

.global asm_inv_sqrt	// export label asm_inv_sqrt

.section .text.rodata

HALF:	.word	0x3f000000		// 0.5f


/*
void asm_inv_sqrt(float *pIn, float *pOut);

R0 = pointer to input float
R1 = pointer to output float
*/
asm_inv_sqrt:
	VLDR.F32			S0, [R0]			// S0 <- input
	VSQRT.F32			S1, S0				// S1 <- sqrt(S0)
	VMOV.F32			S2, #1				// S2 <- 1.0f
	VDIV.F32			S0, S2, S1
	VSTR.F32			S0, [R1]
	BX					LR
