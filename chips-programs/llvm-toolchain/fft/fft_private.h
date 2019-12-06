/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft_private.h
 *
 * Code generated for Simulink model 'fft'.
 *
 * Model version                  : 1.4
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Thu Dec 14 09:36:37 2017
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_fft_private_h_
#define RTW_HEADER_fft_private_h_
#include "rtwtypes.h"

extern void MWDSPCG_R2BRScramble_O_JaPAhAp6(creal32_T y[], const creal32_T x[],
  int32_T nChans, int32_T nRows);
extern void MWDSPCG_R2DIT_TBLS_C(creal32_T y[], int32_T nChans, int32_T nRows,
  int32_T fftLen, int32_T offset, const real32_T tablePtr[], int32_T twiddleStep,
  boolean_T isInverse);

#endif                                 /* RTW_HEADER_fft_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
