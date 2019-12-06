/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft_data.c
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

/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft.h
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

#ifndef RTW_HEADER_fft_h_
#define RTW_HEADER_fft_h_
#include <string.h>
//#include <stddef.h>
#ifndef fft_COMMON_INCLUDES_
# define fft_COMMON_INCLUDES_
/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rtwtypes.h
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

#ifndef RTWTYPES_H
#define RTWTYPES_H

/* Logical type definitions */
#if (!defined(__cplusplus))
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

/*=======================================================================*
 * Target hardware information
 *   Device type: Intel->x86-64 (Windows64)
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       native word size:  64
 *   Byte ordering: LittleEndian
 *   Signed integer division rounds to: Zero
 *   Shift right on a signed integer as arithmetic shift: on
 *=======================================================================*/

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
#define real32_T float
#define real64_T double

/*===========================================================================*
 * Generic type definitions: boolean_T, char_T, byte_T, int_T, uint_T,       *
 *                           real_T, time_T, ulong_T.                        *
 *===========================================================================*/
#define real_T double
#define time_T double
#define boolean_T unsigned char
#define int_T int
#define uint_T unsigned int
#define ulong_T unsigned long
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
 * Min and Max:                                                          *
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

/* Block D-Work pointer type */
#define pointer_T void*

#endif                                 /* RTWTYPES_H */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
#endif                                 /* fft_COMMON_INCLUDES_ */

/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft_types.h
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

#ifndef RTW_HEADER_fft_types_h_
#define RTW_HEADER_fft_types_h_

/* Forward declaration for rtModel */
#define RT_MODEL_fft_T struct tag_RTM_fft_T

#endif                                 /* RTW_HEADER_fft_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<Root>/FFT'
   */
  real32_T FFT_TwiddleTable[192];
} ConstP_fft_T;

/* Real-time Model Data Structure */
struct tag_RTM_fft_T {
  const char_T * errorStatus;
};

/* Constant parameters (auto storage) */
const ConstP_fft_T fft_ConstP;

/* Model entry point functions */
//void fft_initialize(void);
//void fft_terminate(void);

/* Customized model step function */
void fft_custom(const real32_T arg_In1[256], creal32_T arg_Out1[256]);

/* Real-time Model object */
//RT_MODEL_fft_T *const fft_M;

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
 * '<Root>' : 'fft'
 */
#endif                                 /* RTW_HEADER_fft_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
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
/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rtwtypes.h
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

#ifndef RTWTYPES_H
#define RTWTYPES_H

/* Logical type definitions */
#if (!defined(__cplusplus))
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

/*=======================================================================*
 * Target hardware information
 *   Device type: Intel->x86-64 (Windows64)
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       native word size:  64
 *   Byte ordering: LittleEndian
 *   Signed integer division rounds to: Zero
 *   Shift right on a signed integer as arithmetic shift: on
 *=======================================================================*/

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
#define real32_T float
#define real64_T double

/*===========================================================================*
 * Generic type definitions: boolean_T, char_T, byte_T, int_T, uint_T,       *
 *                           real_T, time_T, ulong_T.                        *
 *===========================================================================*/
#define real_T double
#define time_T double
#define boolean_T unsigned char
#define int_T int
#define uint_T unsigned int
#define ulong_T unsigned long
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
 * Min and Max:                                                          *
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

/* Block D-Work pointer type */
#define pointer_T void*

#endif                                 /* RTWTYPES_H */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

void MWDSPCG_R2BRScramble_O_JaPAhAp6(creal32_T y[], const creal32_T x[],
  int32_T nChans, int32_T nRows);
void MWDSPCG_R2DIT_TBLS_C(creal32_T y[], int32_T nChans, int32_T nRows,
  int32_T fftLen, int32_T offset, const real32_T tablePtr[], int32_T twiddleStep,
  boolean_T isInverse);

#endif                                 /* RTW_HEADER_fft_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

/* Constant parameters (auto storage) */
const real32_T twiddleTable[192] =
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<Root>/FFT'
   */
  { 1.0F, 0.999698818F, 0.99879545F, 0.997290432F, 0.99518472F, 0.992479563F,
    0.989176512F, 0.985277653F, 0.980785251F, 0.975702107F, 0.970031261F,
    0.963776052F, 0.956940353F, 0.949528158F, 0.941544056F, 0.932992816F,
    0.923879504F, 0.914209783F, 0.903989315F, 0.893224299F, 0.881921291F,
    0.870086968F, 0.857728601F, 0.84485358F, 0.831469595F, 0.817584813F,
    0.803207517F, 0.78834641F, 0.773010433F, 0.757208824F, 0.740951121F,
    0.724247098F, 0.707106769F, 0.689540565F, 0.671559F, 0.653172851F,
    0.634393275F, 0.615231574F, 0.59569931F, 0.575808167F, 0.555570245F,
    0.534997642F, 0.514102757F, 0.492898196F, 0.471396744F, 0.449611336F,
    0.427555084F, 0.405241311F, 0.382683426F, 0.359895051F, 0.336889863F,
    0.313681751F, 0.290284663F, 0.266712755F, 0.242980182F, 0.219101235F,
    0.195090324F, 0.170961887F, 0.146730468F, 0.122410677F, 0.0980171412F,
    0.0735645667F, 0.0490676761F, 0.024541229F, 6.12323426E-17F, -0.024541229F,
    -0.0490676761F, -0.0735645667F, -0.0980171412F, -0.122410677F, -0.146730468F,
    -0.170961887F, -0.195090324F, -0.219101235F, -0.242980182F, -0.266712755F,
    -0.290284663F, -0.313681751F, -0.336889863F, -0.359895051F, -0.382683426F,
    -0.405241311F, -0.427555084F, -0.449611336F, -0.471396744F, -0.492898196F,
    -0.514102757F, -0.534997642F, -0.555570245F, -0.575808167F, -0.59569931F,
    -0.615231574F, -0.634393275F, -0.653172851F, -0.671559F, -0.689540565F,
    -0.707106769F, -0.724247098F, -0.740951121F, -0.757208824F, -0.773010433F,
    -0.78834641F, -0.803207517F, -0.817584813F, -0.831469595F, -0.84485358F,
    -0.857728601F, -0.870086968F, -0.881921291F, -0.893224299F, -0.903989315F,
    -0.914209783F, -0.923879504F, -0.932992816F, -0.941544056F, -0.949528158F,
    -0.956940353F, -0.963776052F, -0.970031261F, -0.975702107F, -0.980785251F,
    -0.985277653F, -0.989176512F, -0.992479563F, -0.99518472F, -0.997290432F,
    -0.99879545F, -0.999698818F, -1.0F, -0.999698818F, -0.99879545F,
    -0.997290432F, -0.99518472F, -0.992479563F, -0.989176512F, -0.985277653F,
    -0.980785251F, -0.975702107F, -0.970031261F, -0.963776052F, -0.956940353F,
    -0.949528158F, -0.941544056F, -0.932992816F, -0.923879504F, -0.914209783F,
    -0.903989315F, -0.893224299F, -0.881921291F, -0.870086968F, -0.857728601F,
    -0.84485358F, -0.831469595F, -0.817584813F, -0.803207517F, -0.78834641F,
    -0.773010433F, -0.757208824F, -0.740951121F, -0.724247098F, -0.707106769F,
    -0.689540565F, -0.671559F, -0.653172851F, -0.634393275F, -0.615231574F,
    -0.59569931F, -0.575808167F, -0.555570245F, -0.534997642F, -0.514102757F,
    -0.492898196F, -0.471396744F, -0.449611336F, -0.427555084F, -0.405241311F,
    -0.382683426F, -0.359895051F, -0.336889863F, -0.313681751F, -0.290284663F,
    -0.266712755F, -0.242980182F, -0.219101235F, -0.195090324F, -0.170961887F,
    -0.146730468F, -0.122410677F, -0.0980171412F, -0.0735645667F, -0.0490676761F,
    -0.024541229F };

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
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

/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft.h
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

#ifndef RTW_HEADER_fft_h_
#define RTW_HEADER_fft_h_
#include <string.h>
#include <stddef.h>
#ifndef fft_COMMON_INCLUDES_
# define fft_COMMON_INCLUDES_
/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rtwtypes.h
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

#ifndef RTWTYPES_H
#define RTWTYPES_H

/* Logical type definitions */
#if (!defined(__cplusplus))
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

/*=======================================================================*
 * Target hardware information
 *   Device type: Intel->x86-64 (Windows64)
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       native word size:  64
 *   Byte ordering: LittleEndian
 *   Signed integer division rounds to: Zero
 *   Shift right on a signed integer as arithmetic shift: on
 *=======================================================================*/

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
#define real32_T float
#define real64_T double

/*===========================================================================*
 * Generic type definitions: boolean_T, char_T, byte_T, int_T, uint_T,       *
 *                           real_T, time_T, ulong_T.                        *
 *===========================================================================*/
#define real_T double
#define time_T double
#define boolean_T unsigned char
#define int_T int
#define uint_T unsigned int
#define ulong_T unsigned long
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
 * Min and Max:                                                          *
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

/* Block D-Work pointer type */
#define pointer_T void*

#endif                                 /* RTWTYPES_H */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
#endif                                 /* fft_COMMON_INCLUDES_ */

/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: fft_types.h
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

#ifndef RTW_HEADER_fft_types_h_
#define RTW_HEADER_fft_types_h_

/* Forward declaration for rtModel */
#define RT_MODEL_fft_T struct tag_RTM_fft_T

#endif                                 /* RTW_HEADER_fft_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<Root>/FFT'
   */
  real32_T FFT_TwiddleTable[192];
} ConstP_fft_T;

/* Real-time Model Data Structure */
struct tag_RTM_fft_T {
  const char_T * errorStatus;
};

/* Constant parameters (auto storage) */
//const ConstP_fft_T fft_ConstP;

/* Model entry point functions */
//void fft_initialize(void);
//void fft_terminate(void);

/* Customized model step function */
void fft_custom(const real32_T arg_In1[256], creal32_T arg_Out1[256]);

/* Real-time Model object */
//RT_MODEL_fft_T *const fft_M;

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
 * '<Root>' : 'fft'
 */
#endif                                 /* RTW_HEADER_fft_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
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
/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: rtwtypes.h
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

#ifndef RTWTYPES_H
#define RTWTYPES_H

/* Logical type definitions */
#if (!defined(__cplusplus))
#  ifndef false
#   define false                       (0U)
#  endif

#  ifndef true
#   define true                        (1U)
#  endif
#endif

/*=======================================================================*
 * Target hardware information
 *   Device type: Intel->x86-64 (Windows64)
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       native word size:  64
 *   Byte ordering: LittleEndian
 *   Signed integer division rounds to: Zero
 *   Shift right on a signed integer as arithmetic shift: on
 *=======================================================================*/

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
#define real32_T float
#define real64_T double

/*===========================================================================*
 * Generic type definitions: boolean_T, char_T, byte_T, int_T, uint_T,       *
 *                           real_T, time_T, ulong_T.                        *
 *===========================================================================*/
#define real_T double
#define time_T double
#define boolean_T unsigned char
#define int_T int
#define uint_T unsigned int
#define ulong_T unsigned long
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
 * Min and Max:                                                          *
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

/* Block D-Work pointer type */
typedef void * pointer_T;

#endif                                 /* RTWTYPES_H */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

void MWDSPCG_R2BRScramble_O_JaPAhAp6(creal32_T y[], const creal32_T x[],
  int32_T nChans, int32_T nRows);
void MWDSPCG_R2DIT_TBLS_C(creal32_T y[], int32_T nChans, int32_T nRows,
  int32_T fftLen, int32_T offset, const real32_T tablePtr[], int32_T twiddleStep,
  boolean_T isInverse);

#endif                                 /* RTW_HEADER_fft_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

/* Real-time model */
//RT_MODEL_fft_T fft_M_;
//RT_MODEL_fft_T *const fft_M = &fft_M_;
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
      //report(j);
      y[j + yIdx] = x[i + yIdx];
//      report((int)x[i].re);
//      report(j);
//      report((int)&y[j]);

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

  report((int)y[2].re);

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

//  report(1000);
//  report(nChans);
//  report(nRows);
//  report(fftLen);
//  report(offset);
//  report(twiddleStep);
//  report(isInverse);
//  report(1001);

  /* S-Function (sdspfft2): '<Root>/FFT' */
  /* DSP System Toolbox Decimation in Time FFT  */
  /* Computation performed using table lookup  */
  /* Output type: complex real32_T */
  nHalf = (fftLen >> 1) * twiddleStep;
  nQtr = nHalf >> 1;
  fwdInvFactor = isInverse ? -1 : 1;
//  report((int)y[2].re);
//  report((int)&y[2].re);
    report(&y[0]);
    report((int)y[0].re);
    report((int)y[1].re);
    report((int)y[2].re);

  /* For each channel */
  offsetCh = offset;
  for (iCh = 0; iCh < nChans; iCh++) {
//    report(1100 + iCh);
//    report((int)&y[5]);
//    report((int)&y[6].im);
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
//      report(13000 + k);
//      report(14000 + idelta);
      i1 = offsetCh;
//      report((int)y[0].re);
//      report((int)y[1].re);
//      report((int)y[2].re);

      /* Perform the first butterfly in each remaining stage, where no multiply is required. */
//      report(900);
      for (ix = 0; ix < k; ix++) {
        i2 = i1 + idelta;
        tmp_re = y[i2].re;
        tmp_im = y[i2].im;

//        report(ix);
//        report(idelta);
//        report(i2);
//        report(30000 + (int)&y[i2].re);
//        report(30000 + (int)&y[i1].re);

//        report(20000 + i1);
//        report(2000 + (int)&y[i1]);
//        report(2000 + (int)&y[i2]);
        y[i2].re = y[i1].re - tmp_re;
        y[i2].im = y[i1].im - tmp_im;
        y[i1].re += tmp_re;
        y[i1].im += tmp_im;
        i1 += idelta << 1;
      }
//      report((int)y[0].re);
//      report((int)y[2 * idelta].re);
//      report((int)y[3 * idelta].re);
//      report(901);

      istart = offsetCh;

      /* Perform remaining butterflies */
      for (j = kratio; j < nHalf; j += kratio) {
        i1 = istart + 1;
        twidRe = twiddleTable[j];
        twidIm = twiddleTable[j + nQtr] * (real32_T)fwdInvFactor;
//        report(1000 + j);
//        report(1000 + (j + nQtr));
//        report((int)(twidRe * 100.0f));
//        report((int)(twidIm * 100.0f));
//        twidRe = 1.0f;
//        twidIm = 1.0f;
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

#include <stdio.h>

const int input_1 input("input_1");
const int output_1 output("output_1");

/* Model step function */
void fft_custom(const real32_T arg_In1[256], creal32_T arg_Out1[256])
{
  creal32_T rtb_FFT[256];
  creal32_T rtb_RealImagtoComplex[256];
  int32_T i;

  /* RealImagToComplex: '<Root>/Real-Imag to Complex' incorporates:
   *  Constant: '<Root>/Constant'
   *  Inport: '<Root>/In1'
   */
  for (i = 0; i < 256; i++) {
    rtb_RealImagtoComplex[i].re = arg_In1[i];
    //report((int)arg_In1[i]);
    rtb_RealImagtoComplex[i].im = 0.0F;
  }

  report(11);

  /* End of RealImagToComplex: '<Root>/Real-Imag to Complex' */

  /* S-Function (sdspfft2): '<Root>/FFT' */
  MWDSPCG_R2BRScramble_O_JaPAhAp6(&rtb_FFT[0U], &rtb_RealImagtoComplex[0U], 1,
    256);
  report(12);
  report((int)rtb_FFT);
  MWDSPCG_R2DIT_TBLS_C(&rtb_FFT[0U], 1, 256, 256, 0,
                       &twiddleTable[0U], 1, false);
  report(13);

  /* Outport: '<Root>/Out1' */
  //memcpy(&arg_Out1[0], &rtb_FFT[0], sizeof(creal32_T) << 8U);
  memcpy((unsigned*)&arg_Out1[0], (unsigned*)&rtb_FFT[0], 2 << 8U);
  report(14);
  //asm("stop");
//  for(i = 0; i < 256; ++i) {
//    report(i);
//    fput_float(rtb_FFT[i].re, output_1);
//    fput_float(rtb_FFT[i].im, output_1);
//  }
}

void main() {
  report(20);
  real32_T in_buffer[256];
  creal32_T out_buffer[256];
  int i;
  while(1) {
    for(i = 0; i < 256; ++i) {
      in_buffer[i] = fget_float(input_1);
      //report((int)in_buffer[i]);
    }
    report(1);
    fft_custom(in_buffer, out_buffer);
    report(2);
    for(i = 0; i < 256; ++i) {
      fput_float(out_buffer[i].re, output_1);
      fput_float(out_buffer[i].im, output_1);
    }
  }
}

/* Model initialize function */
//void fft_initialize(void)
//{
//  /* Registration code */
//
//  /* initialize error status */
//  //rtmSetErrorStatus(fft_M, (NULL));
//}
//
///* Model terminate function */
//void fft_terminate(void)
//{
//  /* (no terminate code required) */
//}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
