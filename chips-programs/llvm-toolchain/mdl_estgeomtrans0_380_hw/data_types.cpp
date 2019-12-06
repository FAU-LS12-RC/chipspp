/*
 * data_types.cpp
 *
 *  Created on: Apr 6, 2017
 *      Author: streit
 */

//#define ERRNO_SUFFIX 2
#include "data_types.hpp"

//#include <cassert>

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
void rt_InitInfAndNaN(size_t realSize) {
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
boolean_T rtIsInf(real_T value) {
  return (boolean_T) ((value == rtInf || value == rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
boolean_T rtIsInfF(real32_T value) {
  return (boolean_T) (((value) == rtInfF || (value) == rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
boolean_T rtIsNaN(real_T value) {
  return (boolean_T) ((value != value) ? 1U : 0U);
}

/* Test if single-precision value is not a number */
boolean_T rtIsNaNF(real32_T value) {
  return (boolean_T) (((value != value) ? 1U : 0U));
}

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetInf(void) {
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetInfF(void) {
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetMinusInf(void) {
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetMinusInfF(void) {
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetNaN(void) {
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetNaNF(void) {
  IEEESingle nanF = { { 0 } };

  nanF.wordL.wordLuint = 0xFFC00000U;
  return nanF.wordL.wordLreal;
}
int32_T div_nde_s32_floor(int32_T numerator, int32_T denominator) {
  return (
      ((numerator < 0) != (denominator < 0)) && (numerator % denominator != 0) ?
          -1 : 0) + numerator / denominator;
}

int32_T roiCast_UINT32(uint32_T num) {
  /* S-Function (svipstatminmax): '<S2>/2-D Minimum' */
  return (int32_T) num;
}

real32_T rt_roundf_snf(real32_T u)
{
  real32_T y;
  if ((real32_T)fabs(u) < 8.388608E+6F) {
    if (u >= 0.5F) {
      y = (real32_T)floor(u + 0.5F);
    } else if (u > -0.5F) {
      y = u * 0.0F;
    } else {
      y = (real32_T)ceil(u - 0.5F);
    }
  } else {
    y = u;
  }

  return y;
}

real_T rt_roundd_snf(real_T u)
{
  real_T y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}
////////////////////////////////////////////////////////
//
// necessary for estimate geom transform
//
////////////////////////////////////////////////////////
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

  LIBMWVISIONRT_API int_T svd_IsFinite(double x)
  {
	  	  union hx_union
		  {
	  		  int32_T hx[2];
	  		  double x;
		  } hx_u;

          //int32_T hx[2];
          //        memcpy(hx,&x,sizeof(x));
          //return (int32_T) (((uint32_T)((hx[1] & 0x7fffffff)-0x7ff00000)>>31) != 0);
          return (int32_T) (((uint32_T)((hx_u.hx[1] & 0x7fffffff)-0x7ff00000)>>31) != 0);
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

  /*
   * Apply a plane rotation to a vector pair
   */
  static void rot_real(
                int_T     n,      /* number of elements in vectors */
                real_T    c,      /* c part of transform */
                real_T    s,      /* s part of transform */
                real_T    *x,     /* first vector */
                real_T    *y      /* second vector */
                )
  {
      real_T t;

      if (n <= 0) {
          return;
      }

      rot_real_label83:for (;n--;) {
          t  = c * *x + s * *y;
          *y = c * *y - s * *x;
          *x++ = t;
          y++;
      }
  }


  /*
   * Singular value decomposition
   */
  LIBMWVISIONRT_API int_T MWVIP_SVD_D
  (
                     real_T       *x,
                     int_T        n,
                     int_T        p,
                     real_T       *s,
                     real_T       *e,
                     real_T       *work,
                     real_T       *u,
                     real_T       *v,
                     int_T        wantv
                     )
  {
      int_T nm1, np1;
      int_T iter, kase, j, k, kp1;
      int_T l, lp1, lm1, ls, lu;
      int_T m, mm, mm1, mm2;
      int_T info=0;
      int_T nct, ncu, nrt, nml;
      int_T ii;
      uint_T pll, plj, pil;
      real_T t, t2, r;
      real_T sm, smm1, emm1, el, sl, b, c, cs, sn, scale, t1, f;
      real_T test, ztest, snorm, g, shift;
      real_T *pxll, *pxlj, *pel, *pel1, *psl, *pull, *pvll;
      real_T *tp1, *tp2, temp, temp1;
      const real_T One = 1.0, Zero = 0.0;
      real_T eps = EPS_real_T;
      real_T  tiny = MIN_real_T / EPS_real_T;



     /*
      *  ----------------------------------------------------------
      *   reduce x to bidiagonal form, storing the diagonal elements
      *   in s and the super-diagonal elements in e.
      */

      ncu   = MIN(n,p);
      np1  = n + 1;
      nm1  = n - 1;
      nct  = MIN(nm1, p);
      nrt  = MAX(0,MIN(p-2,n));
      lu   = MAX(nct,nrt);
      MWVIP_SVD_D_label39:for (l=0; l<lu; l++) {
          nml = n - l;
          lp1 = l + 1;
          pll = l * np1;
          pxll = x + pll;         /* pointer to x(l,l) */
          psl = s + l;            /* pointer to s(l)   */
          pel = e + l;            /* pointer to e(l)   */
          pel1 = pel + 1;         /* pointer to e(l+1) */
          if (l < nct) {
          /*
          * compute the transformation for the l-th column
          * and place the l-th diagonal in s(l).
              */
              /*  *psl = nrm2(nml,pxll)  */
              *psl = Zero;
              tp1 = pxll;
              MWVIP_SVD_D_label40:for (ii=0; ii<nml; ii++) {
                  CHYPOT(*psl,*tp1,*psl);
                  tp1++;
              }
              /*  *psl = nrm2(nml,pxll)  */
              if (fabs(*psl) != Zero) {
                  if (fabs(*pxll) != Zero) {
                      /*  *psl = sign(*psl,*pxll)  */
                      *psl = (*pxll >= Zero) ? fabs(*psl) : -fabs(*psl);
                      /*  *psl = sign(*psl,*pxll)  */
                  }
                  /*  scal(nml,One / *psl,pxll)  */
                  temp1 = One / *psl;
                  tp1 = pxll;
                  MWVIP_SVD_D_label41:for(ii=0; ii<nml; ii++) {
                      *tp1++ *= temp1;
                  }
                  /*  scal(nml,One / *psl,pxll)  */
                  *pxll += One;
              }
              *psl = -*psl;
          }
          MWVIP_SVD_D_label42:for (j=lp1; j<p; j++) {
              plj = j * n + l;
              pxlj = x + plj;     /* x(l,j) */
              if (l < nct && fabs(*psl) != Zero) {
              /*
              *   Apply the transformation.
                  */
                  /*  t = -dot(nml,pxll,pxlj)  */
                  tp1 = pxll;
                  tp2 = pxlj;
                  t = Zero;
                  MWVIP_SVD_D_label43:for(ii=0; ii<nml; ii++) {
                      t += *tp1++ * *tp2++;
                  }
                  t = -t;
                  /*  t = -dot(nml,pxll,pxlj)  */
                  t /= *pxll;
                  /*  axpy(nml,t,pxll,pxlj)  */
                  tp1 = pxlj;
                  tp2 = pxll;
                  MWVIP_SVD_D_label44:for(ii=0; ii<nml; ii++) {
                      *tp1 += t * *tp2;
                      tp1++;
                      tp2++;
                  }
                  /*  axpy(nml,t,pxll,pxlj)  */
              }
              /*
              * Place the l-th row of x into  e for the
              * subsequent calculation of the row transformation.
              */
              *(e+j) = *pxlj;
          }
          if (wantv && l < nct) {
          /*
          * Place the transformation in u for
          * subsequent back multiplication.
              */
              tp1=u+pll;
              tp2=pxll;
              MWVIP_SVD_D_label45:for (ii=0; ii<nml; ii++) {
                  *tp1++ = *tp2++;
              }
          }
          if (l < nrt) {
          /*
          *       Compute the l-th row transformation and place
          *       the l-th super-diagonal in e(l).
              */
              /*  psl->re = nrm2(p-lp1,pel1)  */
              *pel = Zero;
              tp1 = pel1;
              MWVIP_SVD_D_label46:for(ii=0; ii<p-lp1; ii++) {
                  CHYPOT(*pel,*tp1,*pel);
                  tp1++;
              }
              /*  psl->re = nrm2(p-lp1,pel1)  */
              if (fabs(*pel) != Zero) {
                  if (fabs(*pel1) != Zero) {
                      /*  *pel = sign(*pel,*pel1)  */
                      *pel = (*pel1 >= Zero) ? fabs(*pel) : -fabs(*pel);
                      /*  *pel = sign(*pel,*pel1)  */
                  }
                  /*  scal(p-lp1,One / *pel,pel1)  */
                  temp1 = One / *pel;
                  tp1 = pel1;
                  MWVIP_SVD_D_label47:for(ii=0; ii<p-lp1; ii++) {
                      *tp1 *= temp1;
                      tp1++;
                  }
                  /*  scal(p-lp1,One / *pel,pel1)  */
                  *(pel+1) += One;
              }
              *pel = -*pel;
              if (lp1 < n && fabs(*pel) != Zero) {
              /*
              *   Apply the transformation.
                  */
                  tp1 = work+lp1;
                  MWVIP_SVD_D_label48:for(ii=0; ii<n-lp1; ii++) {
                      *tp1++ = Zero;
                  }
                  MWVIP_SVD_D_label49:for (j=lp1; j<p; j++) {
                      plj = j*n+lp1;              /* x(lp1,j) */
                      /*  axpy(n-lp1,*(e+j),x+plj,work+lp1)  */
                      tp1 = work+lp1;
                      tp2 = x+plj;
                      MWVIP_SVD_D_label50:for(ii=0; ii<n-lp1; ii++) {
                          *tp1++ += *(e+j) * *tp2++;
                      }
                      /*  axpy(n-lp1,*(e+j),x+plj,work+lp1)  */
                  }
                  MWVIP_SVD_D_label51:for (j=lp1; j<p; j++) {
                      t2 = -*(e+j);
                      t = t2 / *pel1;
                      plj = j*n+lp1;              /* x(lp1,j) */
                      /*  axpy(n-lp1,t,work+lp1,x+plj)  */
                      tp1 = x+plj;
                      tp2 = work+lp1;
                      MWVIP_SVD_D_label52:for(ii=0; ii<n-lp1; ii++) {
                          temp = t * *tp2++;
                          *tp1++ += temp;
                      }
                      /*  axpy(n-lp1,t,work+lp1,x+plj)  */
                  }
              }
              if (wantv) {
              /*
              * Place the transformation in v for
              * subsequent back multiplication.
                  */
                  pll = lp1 + l * p;  /* pointer to v(lp1,l) */
                  tp1 = pel1;
                  tp2 = v+pll;
                  MWVIP_SVD_D_label53:for(ii=0; ii<p-lp1; ii++) {
                      *tp2++ = *tp1++;
                  }
              }
          }
          }

          /*
          *-------------------------------------------------------------
          *       set up the final bidiagonal matrix or order m.
          */

          mm1 = m = MIN(p, np1);
          mm1--;
          if (nct < p) {
              pil = nct * np1;    /* pointer to x(nct,nct) */
              *(s+nct) = *(x+pil);
          }
          if (n < m) {
              *(s+mm1) = Zero;
          }
          if (nrt < mm1) {
              pil = nrt + n * mm1;        /* pointer to x(nrt,m) */
              *(e+nrt) = *(x+pil);
          }
          *(e+mm1) = Zero;

          /*
          *-------------------------------------------------------------
          *       if required, generate u.
          */
          if (wantv) {
              MWVIP_SVD_D_label54:for (j=nct; j<ncu; j++) {
                  tp1 = u+j*n;
                  MWVIP_SVD_D_label55:for(ii=0; ii<n; ii++) {
                      *tp1++ = Zero;
                  }
                  *(u+j*np1) = One;
              }
              MWVIP_SVD_D_label57:for (l=nct-1; l>=0; l--) {
                  nml = n - l;
                  pll = l * np1;
                  pull = u + pll; /* u(l,l) */
                  if (fabs(*(s+l)) != Zero) {
                      lp1 = l + 1;
                      MWVIP_SVD_D_label58:for (j=lp1; j<ncu; j++) {
                          plj = j*n+l;    /* u(l,j) */
                          /*  t = -dot(nml,pull,u+plj)  */
                          tp1 = pull;
                          tp2 = u+plj;
                          t = Zero;
                          MWVIP_SVD_D_label59:for(ii=0; ii<nml; ii++) {
                              t += *tp1++ * *tp2++;
                          }
                          t = -t;
                          /*  t = -dot(nml,pull,u+plj)  */
                          t /= *pull;
                          /*  axpy(nml,t,pull,u+plj)  */
                          tp1 = u+plj;
                          tp2 = pull;
                          MWVIP_SVD_D_label60:for(ii=0; ii<nml; ii++) {
                              temp = t * *tp2++;
                              *tp1++ += temp;
                          }
                          /*  axpy(nml,t,pull,u+plj)  */
                      }
                      /*  scal(nml,MinusOne,pull)  */
                      tp1 = pull;
                      MWVIP_SVD_D_label61:for(ii=0; ii<nml; ii++) {
                          *tp1 = -(*tp1);
                          tp1++;
                      }
                      /*  scal(nml,MinusOne,pull)  */
                      *pull += One;
                      if (l >= 1) {
                          tp1 = pull-l;
                          MWVIP_SVD_D_label62:for(ii=0; ii<l; ii++) {
                              *tp1++ = Zero;
                          }
                      }
                  } else {
                      tp1 = pull-l;
                      MWVIP_SVD_D_label63:for(ii=0; ii<n; ii++) {
                          *tp1++ = Zero;
                      }
                      *pull = One;
                  }
              }
          }
          /*
          *-------------------------------------------------------------
          *       If it is required, generate v.
          */
          if (wantv) {
              MWVIP_SVD_D_label64:for (l=p-1; l>=0; l--) {
                  lp1 = l + 1;
                  pll = l*p+lp1;
                  pvll = v + pll; /* v(lp1,l) */
                  if (l < nrt && fabs(*(e+l)) != Zero) {
                      MWVIP_SVD_D_label65:for (j=lp1; j<p; j++) {
                          plj = j*p+lp1;  /* v(lp1,j) */
                          /*  t = -dot(p-lp1,pvll,v+plj)  */
                          tp1 = pvll;
                          tp2 = v+plj;
                          t = Zero;
                          MWVIP_SVD_D_label66:for(ii=0; ii<p-lp1; ii++) {
                              t += *tp1++ * *tp2++;
                          }
                          t = -t;
                          /*  t = -dot(p-lp1,pvll,v+plj)  */
                          t /= *pvll;
                          /*  axpy(p-lp1,t,pvll,v+plj)  */
                          tp1 = v+plj;
                          tp2 = pvll;
                          MWVIP_SVD_D_label67:for(ii=0; ii<p-lp1; ii++) {
                              *tp1++ += t * *tp2++;
                          }
                          /*  axpy(p-lp1,t,pvll,v+plj)  */
                      }
                  }
                  tp1 = pvll-lp1;
                  MWVIP_SVD_D_label68:for(ii=0; ii<p; ii++) {
                      *tp1++ = Zero;
                  }
                  *(pvll-1) = One;                /* v(l,l) */
              }
          }
          /*
          *-------------------------------------------------------------
          *       Transform s and e so that they are real.
          */
          MWVIP_SVD_D_label69:for (l=0; l<m; l++) {
              lp1 = l + 1;
              psl = s + l;                /* pointer to s(l)   */
              pel = e + l;                /* pointer to e(l)   */
              t = fabs(*psl);
              if (t != Zero) {
                  r = *psl / t;
                  *psl = t;
                  if (lp1 < m) {
                      *pel /= r;
                  }
                  if (wantv && l < n) {
                      /*  scal(n,r,u+l*n)  */
                      tp1 = u+l*n;
                      MWVIP_SVD_D_label70:for(ii=0; ii<n; ii++) {
                          *tp1 *= r;
                          tp1++;
                      }
                      /*  scal(n,r,u+l*n)  */
                  }
              }
              if (lp1 == m) break;                /*  ...exit */
              t = fabs(*pel);
              if (t != Zero) {
                  temp = t;
                  r = temp / *pel;
                  *pel = t;
                  psl++;          /* s(l+1) */
                  *psl = *psl * r;
                  if (wantv) {
                      /*  scal(p,r,v+p*lp1)  */
                      tp1 = v+p*lp1;
                      MWVIP_SVD_D_label71:for(ii=0; ii<p; ii++) {
                          *tp1 *= r;
                          tp1++;
                      }
                      /*  scal(p,r,v+p*lp1)  */
                  }
              }
          }

          /*
          *-------------------------------------------------------------
          *       Main iteration loop for the singular values.
          */
          mm   = m;
          iter = 0;
          snorm = 0;
          MWVIP_SVD_D_label72:for (l=0; l<m; l++) {
              snorm = MAX(snorm, MAX(fabs(*(s+l)), fabs(*(e+l))));
          }

          /*
          *       Quit if all the singular values have been found, or
          *       if too many iterations have been performed, set
          *       flag and return.
          */

          MWVIP_SVD_D_label73:for (;m != 0 && iter <= MAXIT;) {
          /*
          *       This section of the program inspects for
          *       negligible elements in the s and e arrays.  On
          *       completion the variable kase is set as follows.
          *
          *       kase = 1     if sr(m) and er(l-1) are negligible
          *                        and l < m
          *       kase = 2     if sr(l) is negligible and l < m
          *       kase = 3     if er(l-1) is negligible, l < m, and
          *                        sr(l), ..., sr(m) are not
          *                        negligible (qr step).
          *       kase = 4     if er(m-1) is negligible (convergence).
              */

              mm1 = m - 1;
              mm2 = m - 2;
              MWVIP_SVD_D_label74:for (l=mm2; l>=0; l--) {
                  test = fabs(*(s+l)) + fabs(*(s+l+1));
                  ztest = fabs(*(e+l));
                  if (!svd_IsFinite(test) || !svd_IsFinite(ztest)) {
                      info = -1;
                      return(info);
                  }
                  if ((ztest <= eps*test) || (ztest <= tiny) ||
                      (iter > 20 && ztest <= eps*snorm)) {
                      *(e+l) = Zero;
                      break;                      /* ...exit */
                  }
              }
              if (l == mm2) {
                  kase = 4;
              } else {
                  lp1 = l + 1;
                  MWVIP_SVD_D_label75:for (ls=m; ls>lp1; ls--) {
                      test = Zero;
                      if (ls != m) test += fabs(*(e+ls-1));
                      if (ls != l + 2) test += fabs(*(e+ls-2));
                      ztest = fabs(*(s+ls-1));
                      if (!svd_IsFinite(test) || !svd_IsFinite(ztest)) {
                          return(info);
                      }
                      if ((ztest <= eps*test) || (ztest <= tiny)) {
                          *(s+ls-1) = Zero;
                          break;                                  /* ...exit */
                      }
                  }
                  if (ls == lp1) {
                      kase = 3;
                  } else if (ls == m) {
                      kase = 1;
                  } else {
                      kase = 2;
                      l = ls - 1;
                  }
              }
              lm1 = ++l - 1;

              /*
              *   Perform the task indicated by kase.
              */
              switch (kase) {

              case 1:                     /* Deflate negligible sr(m). */
                  f = *(e+mm2);
                  *(e+mm2) = Zero;
                  MWVIP_SVD_D_label76:for (k=mm2; k>=l; k--) {
                      t1 = *(s+k);
                      svd_rotg(&t1, &f, &cs, &sn);
                      *(s+k) = t1;
                      if (k != l) {
                          f = -sn * *(e+k-1);
                          *(e+k-1) *= cs;
                      }
                      if (wantv) {
                          rot_real(p, cs, sn, v+k*p, v+mm1*p);
                      }
                  }
                  break;

              case 2:                     /* split at negligible sr(l). */
                  f = *(e+lm1);
                  *(e+lm1) = Zero;
                  MWVIP_SVD_D_label77:for (k=l; k<m; k++) {
                      t1 = *(s+k);
                      svd_rotg(&t1, &f, &cs, &sn);
                      *(s+k) = t1;
                      f = -sn * *(e+k);
                      *(e+k) *= cs;
                      if (wantv) {
                          rot_real(n, cs, sn, u+n*k,u+n*lm1);
                      }
                  }
                  break;

              case 3:                             /* perform one qr step. */
                                                  /*
                                                  *       Calculate the shift.
                  */
                  scale = MAX(fabs(*(s+mm1)), MAX(fabs(*(s+mm2)), fabs(*(e+mm2))));
                  scale = MAX(fabs(scale), MAX(fabs(*(s+l)), fabs(*(e+l))));
                  sm = *(s+mm1) / scale;
                  smm1 = *(s+mm2) / scale;
                  emm1 = *(e+mm2) / scale;
                  sl = *(s+l) / scale;
                  el = *(e+l) / scale;
                  b = ((smm1 + sm) * (smm1 - sm) + emm1 * emm1) / 2.0;
                  c = sm * emm1;
                  c *= c;
                  shift = Zero;
                  if (b != Zero || c != Zero) {
                      shift = sqrt(b * b + c);
                      if (b < Zero) shift = -shift;
                      shift = c / (b + shift);
                  }
                  f = (sl + sm) * (sl - sm) + shift;
                  g = sl * el;
                  /*
                  *       Chase Zeros.
                  */
                  MWVIP_SVD_D_label78:for (k=l; k<mm1; k++) {
                      kp1 = k + 1;
                      svd_rotg(&f, &g, &cs, &sn);
                      if (k != l) *(e+k-1) = f;
                      f = cs * *(s+k) + sn * *(e+k);
                      *(e+k) = cs * *(e+k) - sn * *(s+k);
                      g = sn * *(s+kp1);
                      *(s+kp1) *= cs;
                      if (wantv) {
                          rot_real(p, cs, sn, v+k*p, v+kp1*p);
                      }
                      svd_rotg(&f, &g, &cs, &sn);
                      *(s+k) = f;
                      f = cs * *(e+k) + sn * *(s+kp1);
                      *(s+kp1) = -sn * *(e+k) + cs * *(s+kp1);
                      g = sn * *(e+kp1);
                      *(e+kp1) *= cs;
                      if (wantv && k < nm1) {
                          rot_real(n, cs, sn, u+n*k, u+n*kp1);
                      }
                  }
                  *(e+mm2) = f;
                  ++iter;
                  break;

              case 4:                     /* convergence */
                                          /*
                                          *       Make the singular value positive
                  */
                  if (*(s+l) < Zero) {
                      *(s+l) = -*(s+l);
                      if (wantv) {
                          /*  scal(p,MinusOne,v+l*p)  */
                          tp1 = v+l*p;
                          MWVIP_SVD_D_label79:for(ii=0; ii<p; ii++) {
                              *tp1 = -(*tp1);
                              tp1++;
                          }
                          /*  scal(p,MinusOne,v+l*p)  */
                      }
                  }
                  /*
                  *       Order the singular value.
                  */
                  MWVIP_SVD_D_label80:for (;l != mm-1 && *(s+l) < *(s+l+1);) {
                      lp1 = l + 1;
                      t = *(s+l);
                      *(s+l) = *(s+lp1);
                      *(s+lp1) = t;
                      if (wantv && lp1 < p) {
                          /*  swap(p,v+l*p,v+lp1*p)  */
                          tp1 = v+l*p;
                          tp2 = v+lp1*p;
                          MWVIP_SVD_D_label81:for(ii=0; ii<p; ii++) {
                              temp = *tp1;
                              *tp1++ = *tp2;
                              *tp2++ = temp;
                          }
                          /*  swap(p,v+l*p,v+lp1*p)  */
                      }
                      if (wantv && lp1 < n) {
                          /*  swap(n,u+l*n,u+lp1*n)  */
                          tp1 = u+l*n;
                          tp2 = u+lp1*n;
                          MWVIP_SVD_D_label82:for(ii=0; ii<n; ii++) {
                              temp = *tp1;
                              *tp1++ = *tp2;
                              *tp2++ = temp;
                          }
                          /*  swap(n,u+l*n,u+lp1*n)  */
                      }
                      ++l;
                  }
                  iter = 0;
                  m--;
                  break;

              default:
                  break;
                  }
              info = m;
          }
          return(info);
  }
///////////////////////////////
//
// declaration for est_geom trans functions
//
////////////////////////////////
  void makeConstraintMatrix_Projective_S32_D(const int32_T pts1[], const int32_T
    pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real_T constraint[])
  {
    uint32_T i;
    uint32_T j;
    uint32_T k;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    /* Generate the constraint matrix. */
    j = 0U;
    k = 1U;
    for (i = 0U; i < sampleNum; i++) {
      constraint[k - 1U] = 0.0;
      constraint[k] = 0.0;
      k += 2U;
      constraint[k - 1U] = 0.0;
      constraint[k] = -(real_T)pts1[j + maxSampleNum];
      k += 2U;
      constraint[k - 1U] = -(real_T)pts1[j];
      constraint[k] = -1.0;
      k++;
      constraint[k] = pts2[j];
      constraint[k] *= (real_T)pts1[j + maxSampleNum];
      k++;
      constraint[k] = pts2[j];
      constraint[k] *= (real_T)pts1[j];
      k += 2U;
      constraint[k - 1U] = pts2[j];
      constraint[k] = pts1[j + maxSampleNum];
      k += 2U;
      constraint[k - 1U] = pts1[j];
      constraint[k] = 1.0;
      k += 2U;
      constraint[k - 1U] = 0.0;
      constraint[k] = 0.0;
      k += 2U;
      constraint[k - 1U] = 0.0;
      constraint[k] = -(real_T)pts2[j + maxSampleNum];
      constraint[k] *= (real_T)pts1[j + maxSampleNum];
      k++;
      constraint[k] = -(real_T)pts2[j + maxSampleNum];
      constraint[k] *= (real_T)pts1[j];
      k += 3U;
      constraint[k - 2U] = -(real_T)pts2[j + maxSampleNum];
      j++;
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void normPts_S32_D(const int32_T pts[], const uint32_T samples[], uint32_T
                     ptsNum, uint32_T sampleNum, real_T ptsNorm[], real_T scale[],
                     real_T cents[])
  {
    uint32_T i;
    uint32_T j;
    real_T sumDis;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    /* Normalize the points. */
    cents[0U] = 0.0;
    cents[1U] = 0.0;
    normPts_S32_D_label9:for (i = 0U; i < sampleNum; i++) {
      j = samples[i];
      sumDis = pts[j + ptsNum];
      ptsNorm[i + sampleNum] = sumDis;
      cents[0U] += sumDis;
      sumDis = pts[j];
      ptsNorm[i] = sumDis;
      cents[1U] += sumDis;
    }

    cents[0U] /= (real_T)sampleNum;
    cents[1U] /= (real_T)sampleNum;
    sumDis = 0.0;
    normPts_S32_D_label10:for (i = 0U; i < sampleNum; i++) {
      ptsNorm[i + sampleNum] -= cents[0U];
      ptsNorm[i] -= cents[1U];
      j = i + sampleNum;
      sumDis += sqrt(ptsNorm[j] * ptsNorm[j] + ptsNorm[i] * ptsNorm[i]);
    }

    if (sumDis > 0.0) {
      scale[0U] = 1.4142135623730951;
      scale[0U] *= (real_T)sampleNum;
      scale[0U] /= sumDis;
      normPts_S32_D_label38:for (i = 0U; i < sampleNum + sampleNum; i++) {
        ptsNorm[i] *= scale[0U];
      }
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void makeConstraintMatrix_Projective_D_D(const real_T pts1[], const real_T pts2[],
    uint32_T sampleNum, uint32_T maxSampleNum, real_T constraint[])
  {
    uint32_T i;
    uint32_T j;
    uint32_T k;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    /* Generate the constraint matrix. */
    j = 0U;
    k = 1U;
    makeConstraintMatrix_Projective_D_D_label11:for (i = 0U; i < sampleNum; i++) {
      constraint[k - 1U] = 0.0;
      constraint[k] = 0.0;
      k += 2U;
      constraint[k - 1U] = 0.0;
      constraint[k] = -pts1[j + maxSampleNum];
      k += 2U;
      constraint[k - 1U] = -pts1[j];
      constraint[k] = -1.0;
      k++;
      constraint[k] = pts2[j];
      constraint[k] *= pts1[j + maxSampleNum];
      k++;
      constraint[k] = pts2[j];
      constraint[k] *= pts1[j];
      k += 2U;
      constraint[k - 1U] = pts2[j];
      constraint[k] = pts1[j + maxSampleNum];
      k += 2U;
      constraint[k - 1U] = pts1[j];
      constraint[k] = 1.0;
      k += 2U;
      constraint[k - 1U] = 0.0;
      constraint[k] = 0.0;
      k += 2U;
      constraint[k - 1U] = 0.0;
      constraint[k] = -pts2[j + maxSampleNum];
      constraint[k] *= pts1[j + maxSampleNum];
      k++;
      constraint[k] = -pts2[j + maxSampleNum];
      constraint[k] *= pts1[j];
      k += 3U;
      constraint[k - 2U] = -pts2[j + maxSampleNum];
      j++;
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void QRV2Norm_real_T(const real_T V[], int32_T N, real_T v2norm[])
  {
    int32_T vi;
    int32_T i;
    real_T Vd;
    real_T tmp;
    real_T val;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    val = 0.0;
    vi = 0;
    QRV2Norm_real_T_label34:for (i = 0; i < N; i++) {
      Vd = V[vi];
      if (fabs(val) > fabs(Vd)) {
        tmp = Vd / val;
        val = sqrt(tmp * tmp + 1.0) * fabs(val);
      } else if (Vd == 0.0) {
        val = 0.0;
      } else {
        tmp = val / Vd;
        val = sqrt(tmp * tmp + 1.0) * fabs(Vd);
      }

      vi++;
    }

    v2norm[0U] = val;

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void QRDC_real_T(real_T xVec[], real_T qrAux[], int32_T jpvt[], real_T work[],
                   int32_T M, int32_T N)
  {
    int32_T j;
    int32_T i;
    int32_T l;
    int32_T maxj;
    int32_T pl;
    int32_T pu;
    int32_T mml;
    int32_T px;
    int32_T px2;
    int32_T plj;
    real_T t;
    real_T nrmxl;
    real_T maxnrm;
    real_T tt;
    int32_T minVal;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    pl = 0;
    QRDC_real_T_label21:for (j = 0; j < N; j++) {
      pu = (jpvt[j] > 0);
      if (jpvt[j] < 0) {
        jpvt[j] = -j;
      } else {
        jpvt[j] = j;
      }

      if (pu == 1) {
        if (j != pl) {
          /* Swap columns pl and j */
          px = pl * M;
          px2 = j * M;
          QRDC_real_T_label22:for (i = M; i > 0; i--) {
            t = xVec[px];
            xVec[px] = xVec[px2];
            xVec[px2] = t;
            px++;
            px2++;
          }
        }

        jpvt[j] = jpvt[pl];
        jpvt[pl] = j;
        pl++;
      }
    }

    pu = N - 1;
    QRDC_real_T_label23:for (j = N - 1; j >= 0; j--) {
      if (jpvt[j] < 0) {
        jpvt[j] = -jpvt[j];
        if (j != pu) {
          /* Swap columns pu and j */
          px = pu * M;
          px2 = j * M;
          QRDC_real_T_label24:for (i = M; i > 0; i--) {
            t = xVec[px];
            xVec[px] = xVec[px2];
            xVec[px2] = t;
            px++;
            px2++;
          }

          px = jpvt[pu];
          jpvt[pu] = jpvt[j];
          jpvt[j] = px;
        }

        pu--;
      }
    }

    /* compute the norms of the free columns */
    QRDC_real_T_label25:for (j = pl; j <= pu; j++) {
      QRV2Norm_real_T(&xVec[j * M], M, &maxnrm);
      qrAux[j] = maxnrm;
      work[j] = qrAux[j];
    }

    /* perform the Householder reduction of x */
    if (M < N) {
      minVal = M;
    } else {
      minVal = N;
    }

    QRDC_real_T_label26:for (l = 0; l < minVal; l++) {
      mml = M - l;

      /* locate the column of largest norm and bring it into the pivot position */
      if ((l >= pl) && (l < pu)) {
        maxnrm = 0.0;
        maxj = l;
        QRDC_real_T_label27:for (j = l; j <= pu; j++) {
          if (qrAux[j] > maxnrm) {
            maxnrm = qrAux[j];
            maxj = j;
          }
        }

        if (maxj != l) {
          /* Swap columns l and maxj */
          px = l * M;
          px2 = maxj * M;
          QRDC_real_T_label28:for (i = M; i > 0; i--) {
            t = xVec[px];
            xVec[px] = xVec[px2];
            xVec[px2] = t;
            px++;
            px2++;
          }

          qrAux[maxj] = qrAux[l];
          work[maxj] = work[l];
          px = jpvt[maxj];
          jpvt[maxj] = jpvt[l];
          jpvt[l] = px;
        }
      }

      qrAux[l] = 0.0;
      if (M - 1 != l) {
        /* Compute the Householder transformation for column l */
        maxj = (M + 1) * l;
        px = maxj;
        QRV2Norm_real_T(&xVec[maxj], mml, &nrmxl);
        if (fabs(nrmxl) != 0.0) {
          if (fabs(xVec[maxj]) != 0.0) {
            if (xVec[maxj] >= 0.0) {
              nrmxl = fabs(nrmxl);
            } else {
              nrmxl = -fabs(nrmxl);
            }
          }

          /* Check if it's safe to multiply by 1/nrmxl instead of dividing by nrmxl */
          px2 = maxj;
          if (5.0E-20 * nrmxl != 0.0) {
            maxnrm = 1.0 / nrmxl;
            QRDC_real_T_label29:for (i = mml; i > 0; i--) {
              xVec[px2] *= maxnrm;
              px2++;
            }
          } else {
            QRDC_real_T_label30:for (i = mml; i > 0; i--) {
              xVec[px2] /= nrmxl;
              px2++;
            }
          }

          xVec[maxj]++;

          /* apply the transformation to the remaining columns, updating the norms. */
          QRDC_real_T_label31:for (j = l + 1; j < N; j++) {
            plj = j * M + l;
            px2 = plj;
            t = 0.0;
            QRDC_real_T_label32:for (i = mml; i > 0; i--) {
              t -= xVec[px] * xVec[px2];
              px++;
              px2++;
            }

            px = maxj;
            px2 = plj;
            t /= xVec[maxj];
            QRDC_real_T_label33:for (i = mml; i > 0; i--) {
              xVec[px2] += t * xVec[px];
              px++;
              px2++;
            }

            px = maxj;
            px2 = 0;
            if (qrAux[j] == 0.0) {
              px2 = 1;
            }

            if (!((j < pl) || (j > pu) || (px2 != 0))) {
              maxnrm = fabs(xVec[plj]) / qrAux[j];
              tt = 1.0 - maxnrm * maxnrm;
              if (tt < 0.0) {
                tt = 0.0;
              }

              t = tt;
              maxnrm = qrAux[j] / work[j];
              tt = 0.05 * tt * maxnrm * maxnrm + 1.0;
              if (tt != 1.0) {
                qrAux[j] *= sqrt(t);
              } else {
                QRV2Norm_real_T(&xVec[plj + 1], mml - 1, &maxnrm);
                qrAux[j] = maxnrm;
                work[j] = qrAux[j];
              }
            }
          }

          /* save the transformation. */
          qrAux[l] = xVec[px];
          xVec[px] = -nrmxl;
        }
      }
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void QRCompQy_real_T(real_T qr[], const real_T qrAuxj[], real_T y[], int32_T n,
                       int32_T j)
  {
    real_T j_0;
    int32_T nmj;
    int32_T i;
    int32_T pjj;
    real_T t;
    int32_T pqr;
    int32_T py;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    if (fabs(qrAuxj[0U]) != 0.0) {
      nmj = n - j;
      pjj = (n + 1) * j;
      pqr = pjj;
      j_0 = qr[pjj];
      qr[pjj] = qrAuxj[0U];
      t = 0.0;
      py = 0;
      QRCompQy_real_T_label36:for (i = nmj; i > 0; i--) {
        t -= y[py] * qr[pqr];
        pqr++;
        py++;
      }

      pqr = pjj;
      t /= qr[pjj];
      py = 0;
      QRCompQy_real_T_label37:for (i = nmj; i > 0; i--) {
        y[py] += t * qr[pqr];
        pqr++;
        py++;
      }

      qr[pjj] = j_0;
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void QRSL1_real_T(real_T qr[], const real_T qrAux[], real_T y[], int32_T n,
                    int32_T k)
  {
    int32_T pqraux;
    int32_T py;
    int32_T u1;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    u1 = n - 1;
    if (k < u1) {
      u1 = k;
    }

    u1--;
    if (u1 + 1 != 0) {
      pqraux = u1;
      py = u1;
      QRSL1_real_T_label35:for (;u1 + 1 > 0; u1--) {
        QRCompQy_real_T(qr, &qrAux[pqraux], &y[py], n, u1);
        pqraux--;
        py--;
//        u1--;
      }
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }

  void QRE_real_T(real_T outQ[], real_T outR[], real_T outE[], real_T qrAux[],
                  real_T work[], int32_T jpvt[], real_T sPtr[], int32_T M, int32_T
                  N, boolean_T economy)
  {
    int32_T j;
    int32_T L;
    int32_T pq;
    int32_T ps;

    /* S-Function (svipesttform): '<Root>/est_geom_trans' */
    QRDC_real_T(outR, qrAux, jpvt, work, M, N);

    /* explicitly form q by manipulating identity */
    if (economy) {
      if (M < N) {
        L = M;
      } else {
        L = N;
      }
    } else {
      L = M;
    }

    pq = 0;
    QRE_real_T_label12:for (j = 0; j < M * L; j++) {
      outQ[j] = 0.0;
    }

    QRE_real_T_label13:for (j = 0; j < L; j++) {
      outQ[pq] = 1.0;
      pq = (pq + M) + 1;
    }

    /* Convert cols of identity into cols of q. Use info stored in lower triangle of r and in vector qraux to work on columns of identity matrix I and transform them into q*I(:,j) i.e. the columns of q. */
    pq = 0;
    QRE_real_T_label14:for (j = 0; j < L; j++) {
      QRSL1_real_T(outR, qrAux, &outQ[pq], M, N);
      pq += M;
    }

    if (economy && (M > N)) {
      /* Copy upper triangle of r to s */
      L = 0;
      ps = 0;
      QRE_real_T_label15:for (j = 1; j - 1 < N; j++) {
        QRE_real_T_label16:for (pq = 0; pq < j; pq++) {
          sPtr[ps] = outR[L];
          ps++;
          L++;
        }

        L = (L + M) - j;
        QRE_real_T_label17:for (pq = 0; pq < N - j; pq++) {
          sPtr[ps] = 0.0;
          ps++;
        }
      }
    } else {
      /* Zero strict lower triangle of r */
      L = M * N - 1;
      QRE_real_T_label18:for (j = N; j - 1 >= 0; j--) {
        QRE_real_T_label19:for (pq = M; pq > j; pq--) {
          outR[L] = 0.0;
          L--;
        }

        if (M < j) {
          L -= M;
        } else {
          L -= j;
        }
      }
    }

    /* form permutation vector e */
    L = 0;
    pq = 0;
    QRE_real_T_label20:for (j = 0; j < N; j++) {
      outE[L] = jpvt[pq] + 1;
      L++;
      pq++;
    }

    /* End of S-Function (svipesttform): '<Root>/est_geom_trans' */
  }
