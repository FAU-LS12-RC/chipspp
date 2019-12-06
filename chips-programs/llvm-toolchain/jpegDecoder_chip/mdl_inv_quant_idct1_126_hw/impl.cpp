/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 *
 * Copyright (c) 2004-2017 Hardware-Software-Co-Design, University of
 * Erlangen-Nuremberg. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * --- This software and any associated documentation is provided "as is"
 *
 *  IN NO EVENT SHALL HARDWARE-SOFTWARE-CODESIGN, UNIVERSITY OF ERLANGEN NUREMBERG
 *  BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 *  CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 *  DOCUMENTATION, EVEN IF HARDWARE-SOFTWARE-CODESIGN, UNIVERSITY OF ERLANGEN
 *  NUREMBERG HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  HARDWARE-SOFTWARE-CODESIGN, UNIVERSITY OF ERLANGEN NUREMBERG, SPECIFICALLY
 *  DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED
 *  HEREUNDER IS ON AN "AS IS" BASIS, AND HARDWARE-SOFTWARE-CODESIGN, UNIVERSITY OF
 *  ERLANGEN NUREMBERG HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 *  ENHANCEMENTS, OR MODIFICATIONS.
 *
 * File: mdl_inv_quant_idct1_126.c
 *
 * Code generated for Simulink model 'mdl_inv_quant_idct1_126'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Mon Oct  8 18:58:38 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_inv_quant_idct1_126_hw.hpp"
#ifndef UCHAR_MAX
#include <limits.h>
#endif
#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. 
Consider adjusting Test hardware word size settings on the 
Hardware Implementation pane to match your compiler word sizes as 
defined in limits.h of the compiler. Alternatively, you can 
select the Test hardware is the same as production hardware option and 
select the Enable portable word sizes option on the Code Generation > 
Verification pane for ERT based targets, which will disable the 
preprocessor word size checks.
#endif
#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. 
Consider adjusting Test hardware word size settings on the 
Hardware Implementation pane to match your compiler word sizes as 
defined in limits.h of the compiler. Alternatively, you can 
select the Test hardware is the same as production hardware option and 
select the Enable portable word sizes option on the Code Generation > 
Verification pane for ERT based targets, which will disable the 
preprocessor word size checks.
#endif
#if ( UINT_MAX != (0xFFFFFFFFU) ) || ( INT_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized uint/int. 
Consider adjusting Test hardware word size settings on the 
Hardware Implementation pane to match your compiler word sizes as 
defined in limits.h of the compiler. Alternatively, you can 
select the Test hardware is the same as production hardware option and 
select the Enable portable word sizes option on the Code Generation > 
Verification pane for ERT based targets, which will disable the 
preprocessor word size checks.
#endif
#if 0
/* Skip this size verification because of preprocessor limitation */
#if ( ULONG_MAX != (0xFFFFFFFFFFFFFFFFUL) ) || ( LONG_MAX != (0x7FFFFFFFFFFFFFFFL) )
#error Code was generated for compiler with different sized ulong/long. 
Consider adjusting Test hardware word size settings on the 
Hardware Implementation pane to match your compiler word sizes as 
defined in limits.h of the compiler. Alternatively, you can 
select the Test hardware is the same as production hardware option and 
select the Enable portable word sizes option on the Code Generation > 
Verification pane for ERT based targets, which will disable the 
preprocessor word size checks.
#endif
#endif
#if 0
/* Skip this size verification because of preprocessor limitation */
#if ( ULLONG_MAX != (0xFFFFFFFFFFFFFFFFUL) ) || ( LLONG_MAX != (0x7FFFFFFFFFFFFFFFL) )
#error Code was generated for compiler with different sized ulong_long/long_long. 
Consider adjusting Test hardware word size settings on the 
Hardware Implementation pane to match your compiler word sizes as 
defined in limits.h of the compiler. Alternatively, you can 
select the Test hardware is the same as production hardware option and 
select the Enable portable word sizes option on the Code Generation > 
Verification pane for ERT based targets, which will disable the 
preprocessor word size checks.
#endif
#endif
/* Constant parameters (auto storage) */
const ConstP_mdl_inv_quant_idct1_126_T mdl_inv_quant_idct1_126_ConstP = {
  /* Computed Parameter: idct_TwiddleTable
   * Referenced by: '<S1>/idct'
   */
  { 255, 181, 237, 98, 251, 142, 213, 50 }
};
/* Block states (auto storage) */
extern void MWDSPCG_BitReverseData_Ys16n4(int16_T y[], int32_T N, int32_T stride,
  int32_T startIdx);
extern void MWDSPCG_IntroduceStride(int32_T elem[], int32_T N, int32_T stride,
  int32_T elemIdx);
extern void MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(int16_T y[], const
  int32_T elem[], const int16_T tbl[]);
void MWDSPCG_BitReverseData_Ys16n4(int16_T y[], int32_T N, int32_T stride,
  int32_T startIdx)
{
  int32_T j;
  int32_T iIdx;
  int32_T jIdx;
  int16_T t;
  int32_T i;
  /* S-Function (sdspdct3): '<S1>/idct' */
  j = 0;
  iIdx = startIdx;
  for (i = 0; i < N - 1; i++) {
    /* swap bit-reversed pairs - do not double swap */
    if (i < j) {
      /* swap when i<j */
      jIdx = j * stride + startIdx;
      t = y[iIdx];
      y[iIdx] = y[jIdx];
      y[jIdx] = t;
    }
    jIdx = N >> 1;
    j ^= jIdx;
    while (!((j & jIdx) != 0)) {
      jIdx >>= 1;
      j ^= jIdx;
    }
    iIdx += stride;
  }
  /* End of S-Function (sdspdct3): '<S1>/idct' */
}
void MWDSPCG_IntroduceStride(int32_T elem[], int32_T N, int32_T stride, int32_T
  elemIdx)
{
  int32_T i;
  /* S-Function (sdspdct3): '<S1>/idct' */
  for (i = 0; i < N; i++) {
    /* step by stride starting from elemIdx passed in */
    elem[i] = elemIdx;
    elemIdx += stride;
  }
  /* End of S-Function (sdspdct3): '<S1>/idct' */
}
void MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(int16_T y[], const int32_T
  elem[], const int16_T tbl[])
{
  int16_T c;
  int16_T s;
  int32_T acc;
  int64_T tmp;
  int32_T q1;
  int32_T tmp_0;
  /* S-Function (sdspdct3): '<S1>/idct' */
  /* Stage 1 */
  /* cos(pi/16) */
  c = tbl[4U];
  /* sin(pi/16) */
  s = tbl[7U];
  /* butterfly */
  acc = y[elem[4]] * s;
  tmp_0 = -c;
  if (tmp_0 > 255) {
    tmp_0 = 255;
  } else {
    if (tmp_0 < -256) {
      tmp_0 = -256;
    }
  }
  q1 = y[elem[7]] * tmp_0;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[7]] * s;
  q1 = y[elem[4]] * c;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[7]] = (int16_T)acc;
  y[elem[4]] = (int16_T)tmp_0;
  /* cos(3*pi/16) */
  c = tbl[6U];
  /* sin(3*pi/16) */
  s = tbl[5U];
  /* butterfly */
  acc = y[elem[5]] * c;
  tmp_0 = -s;
  if (tmp_0 > 255) {
    tmp_0 = 255;
  } else {
    if (tmp_0 < -256) {
      tmp_0 = -256;
    }
  }
  q1 = y[elem[6]] * tmp_0;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[6]] * c;
  q1 = y[elem[5]] * s;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[6]] = (int16_T)acc;
  y[elem[5]] = (int16_T)tmp_0;
  /* Stage 2 */
  /* cos(pi/4) */
  c = tbl[1U];
  /* butterfly */
  acc = y[elem[0]] << 8;
  q1 = y[elem[1]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp = (int64_T)acc * c >> 8;
  if (tmp > 2147483647L) {
    tmp = 2147483647L;
  } else {
    if (tmp < -2147483648L) {
      tmp = -2147483648L;
    }
  }
  tmp_0 = (int32_T)tmp >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[0]] << 8;
  q1 = y[elem[1]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  tmp = (int64_T)acc * c >> 8;
  if (tmp > 2147483647L) {
    tmp = 2147483647L;
  } else {
    if (tmp < -2147483648L) {
      tmp = -2147483648L;
    }
  }
  acc = (int32_T)tmp >> 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[1]] = (int16_T)acc;
  y[elem[0]] = (int16_T)tmp_0;
  /* cos(pi/8) */
  c = tbl[2U];
  /* sin(pi/8) */
  s = tbl[3U];
  /* butterfly */
  acc = y[elem[2]] * s;
  tmp_0 = -c;
  if (tmp_0 > 255) {
    tmp_0 = 255;
  } else {
    if (tmp_0 < -256) {
      tmp_0 = -256;
    }
  }
  q1 = y[elem[3]] * tmp_0;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[3]] * s;
  q1 = y[elem[2]] * c;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[3]] = (int16_T)acc;
  y[elem[2]] = (int16_T)tmp_0;
  /* butterfly */
  acc = y[elem[4]] << 8;
  q1 = y[elem[5]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[4]] << 8;
  q1 = y[elem[5]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[5]] = (int16_T)acc;
  y[elem[4]] = (int16_T)tmp_0;
  /* butterfly */
  acc = y[elem[7]] << 8;
  q1 = y[elem[6]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[7]] << 8;
  q1 = y[elem[6]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[7]] = (int16_T)acc;
  y[elem[6]] = (int16_T)tmp_0;
  /* Stage 3 */
  /* butterfly */
  acc = y[elem[0]] << 8;
  q1 = y[elem[3]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[0]] << 8;
  q1 = y[elem[3]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[3]] = (int16_T)acc;
  y[elem[0]] = (int16_T)tmp_0;
  /* butterfly */
  acc = y[elem[1]] << 8;
  q1 = y[elem[2]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[1]] << 8;
  q1 = y[elem[2]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[2]] = (int16_T)acc;
  y[elem[1]] = (int16_T)tmp_0;
  /* cos(pi/4) */
  c = tbl[1U];
  /* butterfly */
  acc = y[elem[6]] << 8;
  q1 = y[elem[5]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  tmp = (int64_T)acc * c >> 8;
  if (tmp > 2147483647L) {
    tmp = 2147483647L;
  } else {
    if (tmp < -2147483648L) {
      tmp = -2147483648L;
    }
  }
  tmp_0 = (int32_T)tmp >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[6]] << 8;
  q1 = y[elem[5]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp = (int64_T)acc * c >> 8;
  if (tmp > 2147483647L) {
    tmp = 2147483647L;
  } else {
    if (tmp < -2147483648L) {
      tmp = -2147483648L;
    }
  }
  acc = (int32_T)tmp >> 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[6]] = (int16_T)acc;
  y[elem[5]] = (int16_T)tmp_0;
  /* Stage 4 */
  /* butterfly */
  acc = y[elem[0]] << 8;
  q1 = y[elem[7]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[0]] << 8;
  q1 = y[elem[7]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[7]] = (int16_T)acc;
  y[elem[0]] = (int16_T)tmp_0;
  /* butterfly */
  acc = y[elem[1]] << 8;
  q1 = y[elem[6]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[1]] << 8;
  q1 = y[elem[6]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[6]] = (int16_T)acc;
  y[elem[1]] = (int16_T)tmp_0;
  /* butterfly */
  acc = y[elem[2]] << 8;
  q1 = y[elem[5]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[2]] << 8;
  q1 = y[elem[5]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[5]] = (int16_T)acc;
  y[elem[2]] = (int16_T)tmp_0;
  /* butterfly */
  acc = y[elem[3]] << 8;
  q1 = y[elem[4]] << 8;
  if ((acc < 0) && (q1 < MIN_int32_T - acc)) {
    acc = MIN_int32_T;
  } else if ((acc > 0) && (q1 > MAX_int32_T - acc)) {
    acc = MAX_int32_T;
  } else {
    acc += q1;
  }
  tmp_0 = acc >> 8;
  if (tmp_0 > 32767) {
    tmp_0 = 32767;
  } else {
    if (tmp_0 < -32768) {
      tmp_0 = -32768;
    }
  }
  acc = y[elem[3]] << 8;
  q1 = y[elem[4]] << 8;
  if ((acc >= 0) && (q1 < acc - MAX_int32_T)) {
    acc = MAX_int32_T;
  } else if ((acc < 0) && (q1 > acc - MIN_int32_T)) {
    acc = MIN_int32_T;
  } else {
    acc -= q1;
  }
  acc >>= 8;
  if (acc > 32767) {
    acc = 32767;
  } else {
    if (acc < -32768) {
      acc = -32768;
    }
  }
  y[elem[4]] = (int16_T)acc;
  y[elem[3]] = (int16_T)tmp_0;
  /* End of S-Function (sdspdct3): '<S1>/idct' */
}
/* Model step function */
void mdl_inv_quant_idct1_126_hw(const int16_T inputArg1[64], uint8_T outputArg1
  [64])
{

DW_mdl_inv_quant_idct1_126_T mdl_inv_quant_idct1_126_DW;

static bool init=true;
if(init){
{
  /* (no initialization code required) */
}
  init=false;
}
  int32_T lowIdx;
  int16_T idct[64];
  int32_T i;
  uint16_T rtb_Switch;
  int16_T tmp;
  /* S-Function (sdspdct3): '<S1>/idct' incorporates:
   *  Product: '<S1>/Product1'
   */
  for (i = 0; i < 64; i++) {
    /* Product: '<S1>/Product1' incorporates:
     *  Inport: '<Root>/In1'
     */
    tmp = inputArg1[i];
    if (inputArg1[i] > 255) {
      tmp = 255;
    } else {
      if (inputArg1[i] < -256) {
        tmp = -256;
      }
    }
    idct[i] = tmp;
  }
  i = 0;
  for (lowIdx = 0; lowIdx < 8; lowIdx++) {
    MWDSPCG_BitReverseData_Ys16n4(&idct[0U], 8, 8, i);
    MWDSPCG_IntroduceStride(&mdl_inv_quant_idct1_126_DW.idct_IDXWKSPACE[0U], 8,
      8, i);
    MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(&idct[0U],
      &mdl_inv_quant_idct1_126_DW.idct_IDXWKSPACE[0U],
      &mdl_inv_quant_idct1_126_ConstP.idct_TwiddleTable[0U]);
    i++;
  }
  i = 0;
  for (lowIdx = 0; lowIdx < 8; lowIdx++) {
    MWDSPCG_BitReverseData_Ys16n4(&idct[0U], 8, 1, i);
    MWDSPCG_IntroduceStride(&mdl_inv_quant_idct1_126_DW.idct_IDXWKSPACE[0U], 8,
      1, i);
    MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(&idct[0U],
      &mdl_inv_quant_idct1_126_DW.idct_IDXWKSPACE[0U],
      &mdl_inv_quant_idct1_126_ConstP.idct_TwiddleTable[0U]);
    i += 8;
  }
  for (i = 0; i < 64; i++) {
    lowIdx = (idct[i] << 6) >> 8;
    /* Switch: '<S2>/Switch' incorporates:
     *  Constant: '<S2>/const'
     *  Constant: '<S2>/const2'
     *  Gain: '<S2>/Gain'
     *  Sum: '<S2>/Add'
     */
    if ((int16_T)lowIdx > 0) {
      rtb_Switch = (uint16_T)((32640 * (int16_T)lowIdx + 2048) >> 11);
    } else {
      rtb_Switch = 0U;
    }
    /* End of Switch: '<S2>/Switch' */
    /* Switch: '<S2>/Switch1' */
    if (rtb_Switch > 255) {
      /* Outport: '<Root>/Out1' incorporates:
       *  Constant: '<S2>/const1'
       */
      outputArg1[i] = MAX_uint8_T;
    } else {
      /* Outport: '<Root>/Out1' */
      outputArg1[i] = (uint8_T)rtb_Switch;
    }
    /* End of Switch: '<S2>/Switch1' */
  }
  /* End of S-Function (sdspdct3): '<S1>/idct' */
}
