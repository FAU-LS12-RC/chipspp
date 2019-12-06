/*
 * vipsvd_rt.h
 * Abstract:
 *   Header file for svd functions
 *
 *  Copyright 1995-2005 The MathWorks, Inc.
 *
 */

#ifndef vipsvd_rt_h
#define vipsvd_rt_h

#include "dsp_rt.h"
//#include "libmwvisionrt_util.h"

/* Maximum number of iterations */
#define MAXIT 75

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Singular value decomposition
 */

/*
 * Function naming glossary
 * ---------------------------
 *
 * MWVIP = MathWorks CVST
 *
 * Data types - (describe inputs to functions, not outputs)
 * R = real single-precision
 * C = complex single-precision
 * D = real double-precision
 * Z = complex double-precision
 */


/*
 * Function naming convention
 * ---------------------------
 *
 * MWVIP_SVD_<DataType>
 *
 * 1) MWVIP is a prefix used with all Mathworks DSP runtime library functions
 *
 * 2) The second field indicates that this function is implementing the SVD
 *    algorithm.
 *
 * 3) The third field is a string indicating the nature of the data type
 *
 */

//LIBMWVISIONRT_API int_T MWVIP_SVD_D(real_T *x,             /*Input matrix*/
//                                 int_T n,       /* M < N ? N : M*/
//                                 int_T p,       /* M < N ? M : N*/
//                                 real_T *s,     /* output svd */
//                                 real_T *e,     /* scratch space for svd algorithm*/
//                                 real_T *work,  /* scratch space for svd algorithm*/
//                                 real_T *u,     /* output pointer for u*/
//                                 real_T *v,     /* output pointer for v*/
//                                 int_T wantv);  /* whether u and v is needed*/
//
//#ifdef CREAL_T
//LIBMWVISIONRT_API int_T MWVIP_SVD_Z(creal_T *x,
//                                 int_T n,
//                                 int_T p,
//                                 creal_T *s,
//                                 creal_T *e,
//                                 creal_T *work,
//                                 creal_T *u,
//                                 creal_T *v,
//                                 int_T wantv);
//#endif /* CREAL_T */

//LIBMWVISIONRT_API int_T MWVIP_SVD_R(real32_T *x,
//int_T MWVIP_SVD_R(real32_T *x,
//                                 int_T n,
//                                 int_T p,
//                                 real32_T *s,
//                                 real32_T *e,
//                                 real32_T *work,
//                                 real32_T *u,
//                                 real32_T *v,
//                                 int_T wantv);

//#ifdef CREAL_T
//LIBMWVISIONRT_API int_T MWVIP_SVD_C(creal32_T *x,
//                                 int_T n,
//                                 int_T p,
//                                 creal32_T *s,
//                                 creal32_T *e,
//                                 creal32_T *work,
//                                 creal32_T *u,
//                                 creal32_T *v,
//                                 int_T wantv);
//#endif /* CREAL_T */
//
///* isFinite */
//LIBMWVISIONRT_API int_T svd_IsFinite(double x);
//LIBMWVISIONRT_API int_T svd_IsFinite32(float x);
//
///* svd_rotg */
//LIBMWVISIONRT_API void svd_rotg(real_T *x, real_T *y, real_T *c, real_T *s);
//LIBMWVISIONRT_API void svd_rotg32(real32_T *x, real32_T *y, real32_T *c, real32_T *s);

#ifdef __cplusplus
}
#endif

#endif /* vipsvd_rt_h */
