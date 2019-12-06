/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft.c
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

#include "fft.h"
#include "fft_private.h"
#include "fft_data.c"

/* Real-time model */
RT_MODEL_fft_T fft_M_;
RT_MODEL_fft_T *const fft_M = &fft_M_;
void MWDSPCG_R2BRScramble_O_JaPAhAp6(creal32_T y[], const creal32_T x[], int32_T
  nChans, int32_T nRows)
{
  int32_T yIdx;
  int32_T j;
  int32_T i;
  int32_T bit_fftLen;

  /* S-Function (sdspfft2): '<Root>/FFT' */
  /* out of place algorithm */
  yIdx = 0;
  while (nChans > 0) {
    nChans--;
    j = 0;

    /* For each element in the source array */
    for (i = 0; i < nRows - 1; i++) {
      /* Copy element into bit-rev position */
      y[j + yIdx] = x[i + yIdx];

      /* Compute next bit-reversed destination index */
      bit_fftLen = nRows;
      do {
        bit_fftLen = (int32_T)((uint32_T)bit_fftLen >> 1);
        j ^= bit_fftLen;
      } while (!((j & bit_fftLen) != 0));
    }

    /* Copy final element */
    y[j + yIdx] = x[i + yIdx];
    yIdx += nRows;
  }

  /* End of S-Function (sdspfft2): '<Root>/FFT' */
}

void MWDSPCG_R2DIT_TBLS_C(creal32_T y[], int32_T nChans, int32_T nRows, int32_T
  fftLen, int32_T offset, const real32_T tablePtr[], int32_T twiddleStep,
  boolean_T isInverse)
{
  int32_T nHalf;
  real32_T twidRe;
  real32_T twidIm;
  int32_T nQtr;
  int32_T fwdInvFactor;
  int32_T iCh;
  int32_T offsetCh;
  int32_T idelta;
  int32_T ix;
  int32_T k;
  int32_T kratio;
  int32_T istart;
  int32_T i1;
  int32_T j;
  int32_T i2;
  real32_T tmp_re;
  real32_T tmp_im;

  /* S-Function (sdspfft2): '<Root>/FFT' */
  /* DSP System Toolbox Decimation in Time FFT  */
  /* Computation performed using table lookup  */
  /* Output type: complex real32_T */
  nHalf = (fftLen >> 1) * twiddleStep;
  nQtr = nHalf >> 1;
  fwdInvFactor = isInverse ? -1 : 1;

  /* For each channel */
  offsetCh = offset;
  for (iCh = 0; iCh < nChans; iCh++) {
    /* Perform butterflies for the first stage, where no multiply is required. */
    for (ix = offsetCh; ix < (fftLen + offsetCh) - 1; ix += 2) {
      tmp_re = y[ix + 1].re;
      tmp_im = y[ix + 1].im;
      y[ix + 1].re = y[ix].re - tmp_re;
      y[ix + 1].im = y[ix].im - tmp_im;
      y[ix].re += tmp_re;
      y[ix].im += tmp_im;
    }

    idelta = 2;
    k = fftLen >> 2;
    kratio = k * twiddleStep;
    while (k > 0) {
      i1 = offsetCh;

      /* Perform the first butterfly in each remaining stage, where no multiply is required. */
      for (ix = 0; ix < k; ix++) {
        i2 = i1 + idelta;
        tmp_re = y[i2].re;
        tmp_im = y[i2].im;
        y[i2].re = y[i1].re - tmp_re;
        y[i2].im = y[i1].im - tmp_im;
        y[i1].re += tmp_re;
        y[i1].im += tmp_im;
        i1 += idelta << 1;
      }

      istart = offsetCh;

      /* Perform remaining butterflies */
      for (j = kratio; j < nHalf; j += kratio) {
        i1 = istart + 1;
        twidRe = tablePtr[j];
        twidIm = tablePtr[j + nQtr] * (real32_T)fwdInvFactor;
        for (ix = 0; ix < k; ix++) {
          i2 = i1 + idelta;
          tmp_re = y[i2].re * twidRe - y[i2].im * twidIm;
          tmp_im = y[i2].re * twidIm + y[i2].im * twidRe;
          y[i2].re = y[i1].re - tmp_re;
          y[i2].im = y[i1].im - tmp_im;
          y[i1].re += tmp_re;
          y[i1].im += tmp_im;
          i1 += idelta << 1;
        }

        istart++;
      }

      idelta <<= 1;
      k >>= 1;
      kratio >>= 1;
    }

    /* Point to next channel */
    offsetCh += nRows;
  }

  /* End of S-Function (sdspfft2): '<Root>/FFT' */
}

/* Model step function */
void fft_custom(const creal32_T rtb_RealImagtoComplex[256], creal32_T rtb_FFT[256])
{
//  creal32_T rtb_FFT[256];
//  creal32_T rtb_RealImagtoComplex[256];
  int32_T i;

  /* RealImagToComplex: '<Root>/Real-Imag to Complex' incorporates:
   *  Constant: '<Root>/Constant'
   *  Inport: '<Root>/In1'
   */
//  for (i = 0; i < 256; i++) {
//    rtb_RealImagtoComplex[i].re = arg_In1[i];
//    rtb_RealImagtoComplex[i].im = 0.0F;
//  }

  /* End of RealImagToComplex: '<Root>/Real-Imag to Complex' */

  /* S-Function (sdspfft2): '<Root>/FFT' */
  MWDSPCG_R2BRScramble_O_JaPAhAp6(&rtb_FFT[0U], &rtb_RealImagtoComplex[0U], 1,
    256);
  MWDSPCG_R2DIT_TBLS_C(&rtb_FFT[0U], 1, 256, 256, 0,
                       &fft_ConstP.FFT_TwiddleTable[0U], 1, false);

  /* Outport: '<Root>/Out1' */
//  memcpy(&arg_Out1[0], &rtb_FFT[0], sizeof(creal32_T) << 8U);
}
//
///* Model initialize function */
//void fft_initialize(void)
//{
//  /* Registration code */
//
//  /* initialize error status */
//  rtmSetErrorStatus(fft_M, (NULL));
//}
//
///* Model terminate function */
//void fft_terminate(void)
//{
//  /* (no terminate code required) */
//}

#include <stdio.h>

const int input_1 input("input_1");
const int output_1 output("output_1");
void main(void) {
  creal32_T in_buffer[256];
  creal32_T out_buffer[256];
  while(1) {
    for(int i = 0; i < 256; ++i) {
      in_buffer[i].re = fget_float(input_1);
      in_buffer[i].im = 0.0f;
    }
    fft_custom(in_buffer, out_buffer);
    for(int i = 0; i < 256; ++i) {
      fput_float(out_buffer[i].re, output_1);
      fput_float(out_buffer[i].im, output_1);
    }
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
