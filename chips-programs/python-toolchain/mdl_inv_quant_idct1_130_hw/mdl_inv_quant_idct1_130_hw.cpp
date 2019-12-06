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
 * File: mdl_inv_quant_idct1_130.c
 *
 * Code generated for Simulink model 'mdl_inv_quant_idct1_130'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Wed May 23 14:41:20 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
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
 * File: mdl_inv_quant_idct1_130.h
 *
 * Code generated for Simulink model 'mdl_inv_quant_idct1_130'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Wed May 23 14:41:20 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_mdl_inv_quant_idct1_130_h_
#define RTW_HEADER_mdl_inv_quant_idct1_130_h_
#ifndef mdl_inv_quant_idct1_130_COMMON_INCLUDES_
# define mdl_inv_quant_idct1_130_COMMON_INCLUDES_
//#include "../data_types.hpp"
#ifndef DATA_TYPES_H
#define DATA_TYPES_H 1

#ifndef TRUE
# define TRUE                          (1U)
#endif

#ifndef FALSE
# define FALSE                         (0U)
#endif

#ifndef __TMWTYPES__
#define __TMWTYPES__

//#include <limits.h>
#define size_t __SIZE_TYPE__

/*=======================================================================*
 * Fixed width word size data types:                                     *
 *   int8_T, int16_T, int32_T     - signed 8, 16, or 32 bit integers     *
 *   uint8_T, uint16_T, uint32_T  - unsigned 8, 16, or 32 bit integers   *
 *   real32_T, real64_T           - 32 and 64 bit floating point numbers *
 *=======================================================================*/
#define int8_T signed char
#define uint8_T unsigned char
#define int16_T short
#define uint16_T unsigned short
#define int32_T int
#define uint32_T unsigned int
#define int64_T long long
#define uint64_T unsigned long long
#define real32_T float
#define real64_T double

/*===========================================================================*
 * Generic type definitions: real_T, time_T, boolean_T, int_T, uint_T,       *
 *                           ulong_T, char_T and byte_T.                     *
 *===========================================================================*/
#define real_T double
#define time_T double
#define boolean_T unsigned char
#define int_T int
#define uint_T unsigned int
#define ulong_T unsigned
#define ulonglong_T unsigned long long
#define char_T char
#define uchar_T unsigned char
#define byte_T char_T

/*===========================================================================*
 * Complex number type definitions                                           *
 *===========================================================================*/
#define CREAL_T

typedef struct {
	real32_T re;
	real32_T im;
} creal32_T;

typedef struct {
	real64_T re;
	real64_T im;
} creal64_T;

typedef struct {
	real_T re;
	real_T im;
} creal_T;

#define CINT8_T

typedef struct {
	int8_T re;
	int8_T im;
} cint8_T;

#define CUINT8_T

typedef struct {
	uint8_T re;
	uint8_T im;
} cuint8_T;

#define CINT16_T

typedef struct {
	int16_T re;
	int16_T im;
} cint16_T;

#define CUINT16_T

typedef struct {
	uint16_T re;
	uint16_T im;
} cuint16_T;

#define CINT32_T

typedef struct {
	int32_T re;
	int32_T im;
} cint32_T;

#define CUINT32_T

typedef struct {
	uint32_T re;
	uint32_T im;
} cuint32_T;

/*=======================================================================*
*   Min and Max:                                                          *
*   int8_T, int16_T, int32_T     - signed 8, 16, or 32 bit integers     *
*   uint8_T, uint16_T, uint32_T  - unsigned 8, 16, or 32 bit integers   *
*=======================================================================*/
#define MAX_int8_T                     ((int8_T)(127))
#define MIN_int8_T                     ((int8_T)(-128))
#define MAX_uint8_T                    ((uint8_T)(255U))
#define MAX_int16_T                    ((int16_T)(32767))
#define MIN_int16_T                    ((int16_T)(-32768))
#define MAX_uint16_T                   ((uint16_T)(65535U))
#define MAX_int32_T                    ((int32_T)(2147483647))
#define MIN_int32_T                    ((int32_T)(-2147483647-1))
#define MAX_uint32_T                   ((uint32_T)(0xFFFFFFFFU))
#define MAX_int64_T                    ((int64_T)(9223372036854775807L))
#define MIN_int64_T                    ((int64_T)(-9223372036854775807L-1L))
#define MAX_uint64_T                   ((uint64_T)(0xFFFFFFFFFFFFFFFFUL))

/* Logical type definitions */
#if (!defined(__cplusplus)) && (!defined(__true_false_are_keywords))
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

#else                                  /* __TMWTYPES__ */
#define TMWTYPES_PREVIOUSLY_INCLUDED
#endif                                 /* __TMWTYPES__ */

/* Block D-Work pointer type */
//typedef void * pointer_T;

/* Simulink specific types */
#ifndef __ZERO_CROSSING_TYPES__
#define __ZERO_CROSSING_TYPES__

/* Trigger directions: falling, either, and rising */
//typedef enum {
//	FALLING_ZERO_CROSSING = -1, ANY_ZERO_CROSSING = 0, RISING_ZERO_CROSSING = 1
//} ZCDirection;

/* Previous state of a trigger signal */
#define ZCSigState uint8_T

/* Initial value of a trigger zero crossing signal */
#define UNINITIALIZED_ZCSIG            0x03U
#define NEG_ZCSIG                      0x02U
#define POS_ZCSIG                      0x01U
#define ZERO_ZCSIG                     0x00U

/* Current state of a trigger signal */
//typedef enum {
//	FALLING_ZCEVENT = -1, NO_ZCEVENT = 0, RISING_ZCEVENT = 1
//} ZCEventType;

#define NumBitsPerChar                 8U
//extern "C" {
//extern real_T rtInf;
//extern real_T rtMinusInf;
//extern real_T rtNaN;
//extern real32_T rtInfF;
//extern real32_T rtMinusInfF;
//extern real32_T rtNaNF;
//extern void rt_InitInfAndNaN(size_t realSize);
//extern boolean_T rtIsInf(real_T value);
//extern boolean_T rtIsInfF(real32_T value);
//extern boolean_T rtIsNaN(real_T value);
//extern boolean_T rtIsNaNF(real32_T value);
//typedef struct {
//	struct {
//		uint32_T wordH;
//		uint32_T wordL;
//	} words;
//} BigEndianIEEEDouble;
//
//typedef struct {
//	struct {
//		uint32_T wordL;
//		uint32_T wordH;
//	} words;
//} LittleEndianIEEEDouble;
//
//typedef struct {
//	union {
//		real32_T wordLreal;
//		uint32_T wordLuint;
//	} wordL;
//} IEEESingle;
//} /* extern "C" */
//
//extern "C" {
//extern real_T rtGetInf(void);
//extern real32_T rtGetInfF(void);
//extern real_T rtGetMinusInf(void);
//extern real32_T rtGetMinusInfF(void);
//} /* extern "C" */
//
//extern "C" {
//extern real_T rtGetNaN(void);
//extern real32_T rtGetNaNF(void);
//} /* extern "C" */

#endif
#endif
#endif                                 /* mdl_inv_quant_idct1_130_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */

typedef struct {
  int32_T idct_IDXWKSPACE[8];          /* '<S1>/idct' */
} DW_mdl_inv_quant_idct1_130_T;

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: idct_TwiddleTable
   * Referenced by: '<S1>/idct'
   */
  int16_T idct_TwiddleTable[8];
} ConstP_mdl_inv_quant_idct1_130_T;


/* Constant parameters (auto storage) */
//ConstP_mdl_inv_quant_idct1_130_T mdl_inv_quant_idct1_130_ConstP;


/* Customized model step function */
void mdl_inv_quant_idct1_130_hw(const int16_T inputArg1[64], uint8_T
  outputArg1[64]);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'mdl_inv_quant_idct1_130'
 * '<S1>'   : 'mdl_inv_quant_idct1_130/inv_quant_idct1'
 * '<S2>'   : 'mdl_inv_quant_idct1_130/inv_quant_idct1/convUint8'
 */
#endif                                 /* RTW_HEADER_mdl_inv_quant_idct1_130_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
#ifndef UCHAR_MAX
//#include <limits.h>
#endif
//#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
//#error Code was generated for compiler with different sized uchar/char. 
//Consider adjusting Test hardware word size settings on the 
//Hardware Implementation pane to match your compiler word sizes as 
//defined in limits.h of the compiler. Alternatively, you can 
//select the Test hardware is the same as production hardware option and 
//select the Enable portable word sizes option on the Code Generation > 
//Verification pane for ERT based targets, which will disable the 
//preprocessor word size checks.
//#endif
//#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
//#error Code was generated for compiler with different sized ushort/short. 
//Consider adjusting Test hardware word size settings on the 
//Hardware Implementation pane to match your compiler word sizes as 
//defined in limits.h of the compiler. Alternatively, you can 
//select the Test hardware is the same as production hardware option and 
//select the Enable portable word sizes option on the Code Generation > 
//Verification pane for ERT based targets, which will disable the 
//preprocessor word size checks.
//#endif
//#if ( UINT_MAX != (0xFFFFFFFFU) ) || ( INT_MAX != (0x7FFFFFFF) )
//#error Code was generated for compiler with different sized uint/int. 
//Consider adjusting Test hardware word size settings on the 
//Hardware Implementation pane to match your compiler word sizes as 
//defined in limits.h of the compiler. Alternatively, you can 
//select the Test hardware is the same as production hardware option and 
//select the Enable portable word sizes option on the Code Generation > 
//Verification pane for ERT based targets, which will disable the 
//preprocessor word size checks.
//#endif
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
const int16_T mdl_inv_quant_idct1_130_ConstP[8] = {
  /* Computed Parameter: idct_TwiddleTable
   * Referenced by: '<S1>/idct'
   */
  255, 181, 237, 98, 251, 142, 213, 50
};
/* Block states (auto storage) */
void MWDSPCG_BitReverseData_Ys16n4(int16_T y[], int32_T N, int32_T stride,
  int32_T startIdx);
void MWDSPCG_IntroduceStride(int32_T elem[], int32_T N, int32_T stride,
  int32_T elemIdx);
void MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(int16_T y[], const
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
//  int i;
//  for(i = 0; i < 8; ++i)
//    report(elem[i]);
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
  //report(c);
  //report(acc);
  tmp = (int64_T)acc * c >> 8;
  //report(tmp);
  if (tmp > 2147483647L) {
    tmp = 2147483647L;
  } else {
    if (tmp < -2147483648L) {
      tmp = -2147483648L;
    }
  }
  //report(tmp);
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
void mdl_inv_quant_idct1_130_hw(const int16_T inputArg1[64], uint8_T outputArg1
  [64])
{

DW_mdl_inv_quant_idct1_130_T mdl_inv_quant_idct1_130_DW;

//static bool init=true;
//if(init){
//{
//  /* (no initialization code required) */
//}
//  init=false;
//}
  int32_T lowIdx;
  int16_T idct[64];
  int32_T i;
  uint16_T rtb_Switch;
  int16_T tmp;
  //report(&idct[0U]);
  report(&mdl_inv_quant_idct1_130_DW);
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
    MWDSPCG_IntroduceStride(&mdl_inv_quant_idct1_130_DW.idct_IDXWKSPACE[0U], 8,
      8, i);
    MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(&idct[0U],
      &mdl_inv_quant_idct1_130_DW.idct_IDXWKSPACE[0U],
      &mdl_inv_quant_idct1_130_ConstP[0U]);
    i++;
  }
  i = 0;
  for (lowIdx = 0; lowIdx < 8; lowIdx++) {
    MWDSPCG_BitReverseData_Ys16n4(&idct[0U], 8, 1, i);
    MWDSPCG_IntroduceStride(&mdl_inv_quant_idct1_130_DW.idct_IDXWKSPACE[0U], 8,
      1, i);
    MWDSPCG_Idct8_Ys16n4_Ts9n8_As32n12_Ps32n12_f_s(&idct[0U],
      &mdl_inv_quant_idct1_130_DW.idct_IDXWKSPACE[0U],
      &mdl_inv_quant_idct1_130_ConstP[0U]);
    i += 8;
  }
  report(&idct[0U]);
  for (i = 0; i < 64; i++) {
    //report(i);
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
/* Model initialize function */
/* Model terminate function */
/*
 * File trailer for generated code.
 *
 * [EOF]
 */

int input_1 = input("input_1");
int output_1 = output("output_1");
void main() {
  int i;
  int16_T inputArg1[64];
  uint8_T outputArg1[64];

  for(i = 0; i < 64; ++i) {
    inputArg1[i] = fgetc(input_1);
  }
  mdl_inv_quant_idct1_130_hw(inputArg1, outputArg1);
  for(i = 0; i < 64; ++i)
    fputc(outputArg1[i], output_1);
}
