/*
 * SVD_HELPER_RT - DSP System Toolbox Singular Value Decomposition helper functions
 *
 *  Copyright 1995-2015 The MathWorks, Inc.
 */

#if (!defined(INTEGER_CODE) || !INTEGER_CODE)

#include "vipsvd_rt.h"
//#include "vipendian_rt.h"
#include <math.h>
#include <string.h>

bool isLittleEndian() {
  return false;
}

LIBMWVISIONRT_API int_T svd_IsFinite(double x)
{
	int32_T hx[2];

	if (isLittleEndian()) {
		memcpy(hx,&x,sizeof(x));
		return (int32_T) (((uint32_T)((hx[1] & 0x7fffffff)-0x7ff00000)>>31) != 0);
	} else {
		memcpy(hx,&x,2*sizeof(int32_T));
		return (int32_T) (((uint32_T)((hx[0] & 0x7fffffff)-0x7ff00000)>>31) != 0);
	}
}

LIBMWVISIONRT_API int_T svd_IsFinite32(float x)
{
	int32_T hx;
	memcpy(&hx,&x,sizeof(x));
    return (int32_T) ((uint32_T)((hx & 0x7fffffff) - 0x7f800000) >> 31 != 0);
}

/* construct Givens plane rotation */
LIBMWVISIONRT_API void svd_rotg(real_T *x, real_T *y, real_T *c, real_T *s)
{
    real_T rho, r, z, absx, absy;

    rho = ((absx = fabs(*x)) > (absy = fabs(*y))) ? *x : *y;
    CHYPOT(*x, *y, r);
    r   = (rho > 0.0) ? r : -r;
    *c  = (r == 0.0) ? 1.0 : *x / r;
    *s  = (r == 0.0) ? 0.0 : *y / r;
    z   = (absx > absy) ? *s : 1.0;
    z   = (absy >= absx && *c != 0.0) ? 1.0 / *c : z;
    *x = r;
    *y = z;
}

/* construct Givens plane rotation */
LIBMWVISIONRT_API void svd_rotg32(real32_T *x, real32_T *y, real32_T *c, real32_T *s)
{
    real32_T rho, r, z, absx, absy;

    rho = ((absx = fabsf(*x)) > (absy = fabsf(*y))) ? *x : *y;
    CHYPOT32(*x, *y, r);
    r   = (rho > 0.0F) ? r : -r;
    *c  = (r == 0.0F) ? 1.0F : *x / r;
    *s  = (r == 0.0F) ? 0.0F : *y / r;
    z   = (absx > absy) ? *s : 1.0F;
    z   = (absy >= absx && *c != 0.0F) ? 1.0F / *c : z;
    *x = r;
    *y = z;
}

#endif /* !INTEGER_CODE */


/* [EOF] svd_d_rt.c */
