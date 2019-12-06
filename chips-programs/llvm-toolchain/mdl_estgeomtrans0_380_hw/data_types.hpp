/*
 * data_types.hpp
 *
 *  Created on: Nov 16, 2016
 *      Author: letras
 */


#ifndef DATA_TYPES_H
#define DATA_TYPES_H 1

#define in_frame 1024
#define out_frame 513

//#include <iostream>
#include <stddef.h>
#include <limits.h>
//#include <cassert>
#include "dsp_rt.h"
//#include "libmwvisionrt_util.h"

#ifndef TRUE
# define TRUE                          (1U)
#endif

#ifndef FALSE
# define FALSE                         (0U)
#endif

#ifndef __TMWTYPES__
#define __TMWTYPES__

//template<typename T, int SIZE>
//class Token {
//public:
//  T Data[SIZE];
//  // default constructor is needed for software synthese
//  Token();
//
//  T &operator[](int index);
//};
//
//template<typename T, int SIZE>
//inline std::ostream &operator <<(std::ostream &out,
//    const Token<T, SIZE> &token) {
//  out << "...";
//  return out;
//}
//
//template<typename T, int SIZE>
//Token<T, SIZE>::Token() {
//
//}
//
//template<typename T, int SIZE>
//T &Token<T, SIZE>::operator[](int index) {
//  assert(0 <= index && index < SIZE);
//  return Token<T, SIZE>::Data[index];
//}

/*=======================================================================*
 * Target hardware information
 *   Device type: 32-bit Generic
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       native word size:  32
 *   Byte ordering: Unspecified
 *   Signed integer division rounds to: Undefined
 *   Shift right on a signed integer as arithmetic shift: on
 *=======================================================================*/
#define NumBitsPerChar                 8U
/*=======================================================================*
 * Fixed width word size data types:                                     *
 *   int8_T, int16_T, int32_T     - signed 8, 16, or 32 bit integers     *
 *   uint8_T, uint16_T, uint32_T  - unsigned 8, 16, or 32 bit integers   *
 *   real32_T, real64_T           - 32 and 64 bit floating point numbers *
 *=======================================================================*/
typedef signed char int8_T;
typedef unsigned char uint8_T;
typedef short int16_T;
typedef unsigned short uint16_T;
typedef int int32_T;
typedef unsigned int uint32_T;
typedef float real32_T;
typedef double real64_T;

/*===========================================================================*
 * Generic type definitions: real_T, time_T, boolean_T, int_T, uint_T,       *
 *                           ulong_T, char_T and byte_T.                     *
 *===========================================================================*/
typedef double real_T;
typedef double time_T;
typedef unsigned char boolean_T;
typedef int int_T;
typedef unsigned int uint_T;
typedef unsigned long ulong_T;
typedef char char_T;
typedef unsigned char uchar_T;
typedef char_T byte_T;

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
#define MIN_uint8_T                    ((uint8_T)(0U))
#define MAX_int16_T                    ((int16_T)(32767))
#define MIN_int16_T                    ((int16_T)(-32768))
#define MAX_uint16_T                   ((uint16_T)(65535U))
#define MIN_uint16_T                   ((uint16_T)(0U))
#define MAX_int32_T                    ((int32_T)(2147483647))
#define MIN_int32_T                    ((int32_T)(-2147483647-1))
#define MAX_uint32_T                   ((uint32_T)(0xFFFFFFFFU))
#define MIN_uint32_T                   ((uint32_T)(0U))

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
typedef void * pointer_T;

/* Simulink specific types */
#ifndef __ZERO_CROSSING_TYPES__
#define __ZERO_CROSSING_TYPES__

/* Trigger directions: falling, either, and rising */
typedef enum {
  FALLING_ZERO_CROSSING = -1, ANY_ZERO_CROSSING = 0, RISING_ZERO_CROSSING = 1
} ZCDirection;

/* Previous state of a trigger signal */
typedef uint8_T ZCSigState;

/* Initial value of a trigger zero crossing signal */
#define UNINITIALIZED_ZCSIG            0x03U
#define NEG_ZCSIG                      0x02U
#define POS_ZCSIG                      0x01U
#define ZERO_ZCSIG                     0x00U

/* Current state of a trigger signal */
typedef enum {
  FALLING_ZCEVENT = -1, NO_ZCEVENT = 0, RISING_ZCEVENT = 1
} ZCEventType;

extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
extern void rt_InitInfAndNaN(size_t realSize);
extern boolean_T rtIsInf(real_T value);
extern boolean_T rtIsInfF(real32_T value);
extern boolean_T rtIsNaN(real_T value);
extern boolean_T rtIsNaNF(real32_T value);
typedef struct {
  struct {
    uint32_T wordH;
    uint32_T wordL;
  } words;
} BigEndianIEEEDouble;

typedef struct {
  struct {
    uint32_T wordL;
    uint32_T wordH;
  } words;
} LittleEndianIEEEDouble;

typedef struct {
  union {
    real32_T wordLreal;
    uint32_T wordLuint;
  } wordL;
} IEEESingle;

extern real_T rtGetInf(void);
extern real32_T rtGetInfF(void);
extern real_T rtGetMinusInf(void);
extern real32_T rtGetMinusInfF(void);

extern real_T rtGetNaN(void);
extern real32_T rtGetNaNF(void);
extern real32_T rt_roundf_snf(real32_T u);
extern real_T rt_roundd_snf(real_T u);

extern int32_T roiCast_UINT32(uint32_T num);
int32_T div_nde_s32_floor(int32_T numerator, int32_T denominator);

////////////////////////////////////////////////////////
//
// necessary for estimate geom transform
//
////////////////////////////////////////////////////////
#ifdef _MSC_VER
    #define LIBMWVISIONRT_API __declspec(dllexport)
#else
    #define LIBMWVISIONRT_API
#endif

LIBMWVISIONRT_API int_T MWVIP_SVD_D(real_T *x,             /*Input matrix*/
                                 int_T n,       /* M < N ? N : M*/
                                 int_T p,       /* M < N ? M : N*/
                                 real_T *s,     /* output svd */
                                 real_T *e,     /* scratch space for svd algorithm*/
                                 real_T *work,  /* scratch space for svd algorithm*/
                                 real_T *u,     /* output pointer for u*/
                                 real_T *v,     /* output pointer for v*/
                                 int_T wantv);  /* whether u and v is needed*/

#ifdef CREAL_T
LIBMWVISIONRT_API int_T MWVIP_SVD_Z(creal_T *x,
                                 int_T n,
                                 int_T p,
                                 creal_T *s,
                                 creal_T *e,
                                 creal_T *work,
                                 creal_T *u,
                                 creal_T *v,
                                 int_T wantv);
#endif /* CREAL_T */

LIBMWVISIONRT_API int_T MWVIP_SVD_R(real32_T *x,
                                 int_T n,
                                 int_T p,
                                 real32_T *s,
                                 real32_T *e,
                                 real32_T *work,
                                 real32_T *u,
                                 real32_T *v,
                                 int_T wantv);

#ifdef CREAL_T
LIBMWVISIONRT_API int_T MWVIP_SVD_C(creal32_T *x,
                                 int_T n,
                                 int_T p,
                                 creal32_T *s,
                                 creal32_T *e,
                                 creal32_T *work,
                                 creal32_T *u,
                                 creal32_T *v,
                                 int_T wantv);
#endif /* CREAL_T */

/* isFinite */
LIBMWVISIONRT_API int_T svd_IsFinite(double x);
LIBMWVISIONRT_API int_T svd_IsFinite32(float x);

/* svd_rotg */
LIBMWVISIONRT_API void svd_rotg(real_T *x, real_T *y, real_T *c, real_T *s);
LIBMWVISIONRT_API void svd_rotg32(real32_T *x, real32_T *y, real32_T *c, real32_T *s);


/* Maximum number of iterations */
#define MAXIT 75
extern void makeConstraintMatrix_Projective_S32_D(const int32_T pts1[], const
  int32_T pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real_T constraint[]);
extern void normPts_S32_D(const int32_T pts[], const uint32_T samples[],
  uint32_T ptsNum, uint32_T sampleNum, real_T ptsNorm[], real_T scale[], real_T
  cents[]);
extern void makeConstraintMatrix_Projective_D_D(const real_T pts1[], const
  real_T pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real_T constraint[]);
extern void QRV2Norm_real_T(const real_T V[], int32_T N, real_T v2norm[]);
extern void QRDC_real_T(real_T xVec[], real_T qrAux[], int32_T jpvt[], real_T
  work[], int32_T M, int32_T N);
extern void QRCompQy_real_T(real_T qr[], const real_T qrAuxj[], real_T y[],
  int32_T n, int32_T j);
extern void QRSL1_real_T(real_T qr[], const real_T qrAux[], real_T y[], int32_T
  n, int32_T k);
extern void QRE_real_T(real_T outQ[], real_T outR[], real_T outE[], real_T
  qrAux[], real_T work[], int32_T jpvt[], real_T sPtr[], int32_T M, int32_T N,
  boolean_T economy);
#endif
#endif
