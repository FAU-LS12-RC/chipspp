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
 * File: mdl_estgeomtrans0_380.c
 *
 * Code generated for Simulink model 'mdl_estgeomtrans0_380'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Tue Jan  8 12:39:56 2019
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM 9
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_estgeomtrans0_380_hw.hpp"
/* Block states (auto storage) */
extern void makeConstraintMatrix_Projective_S16_R(const int16_T pts1[], const
  int16_T pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real32_T
  constraint[]);
extern void normPts_S16_R(const int16_T pts[], const uint32_T samples[],
  uint32_T ptsNum, uint32_T sampleNum, real32_T ptsNorm[], real32_T scale[],
  real32_T cents[]);
extern void makeConstraintMatrix_Projective_R_R(const real32_T pts1[], const
  real32_T pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real32_T
  constraint[]);
extern void QRV2Norm_real32_T(const real32_T V[], int32_T N, real32_T v2norm[]);
extern void QRDC_real32_T(real32_T xVec[], real32_T qrAux[], int32_T jpvt[],
  real32_T work[], int32_T M, int32_T N);
extern void QRCompQy_real32_T(real32_T qr[], const real32_T qrAuxj[], real32_T
  y[], int32_T n, int32_T j);
extern void QRSL1_real32_T(real32_T qr[], const real32_T qrAux[], real32_T y[],
  int32_T n, int32_T k);
extern void QRE_real32_T(real32_T outQ[], real32_T outR[], real32_T outE[],
  real32_T qrAux[], real32_T work[], int32_T jpvt[], real32_T sPtr[], int32_T M,
  int32_T N, boolean_T economy);
void makeConstraintMatrix_Projective_S16_R(const int16_T pts1[], const int16_T
  pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real32_T constraint[])
{
  uint32_T i;
  uint32_T j;
  uint32_T k;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  /* Generate the constraint matrix. */
  j = 0U;
  k = 1U;
  for (i = 0U; i < sampleNum; i++) {
    constraint[k - 1U] = 0.0F;
    constraint[k] = 0.0F;
    k += 2U;
    constraint[k - 1U] = 0.0F;
    constraint[k] = -(real32_T)pts1[j + maxSampleNum];
    k += 2U;
    constraint[k - 1U] = -(real32_T)pts1[j];
    constraint[k] = -1.0F;
    k++;
    constraint[k] = pts2[j];
    constraint[k] *= (real32_T)pts1[j + maxSampleNum];
    k++;
    constraint[k] = pts2[j];
    constraint[k] *= (real32_T)pts1[j];
    k += 2U;
    constraint[k - 1U] = pts2[j];
    constraint[k] = pts1[j + maxSampleNum];
    k += 2U;
    constraint[k - 1U] = pts1[j];
    constraint[k] = 1.0F;
    k += 2U;
    constraint[k - 1U] = 0.0F;
    constraint[k] = 0.0F;
    k += 2U;
    constraint[k - 1U] = 0.0F;
    constraint[k] = -(real32_T)pts2[j + maxSampleNum];
    constraint[k] *= (real32_T)pts1[j + maxSampleNum];
    k++;
    constraint[k] = -(real32_T)pts2[j + maxSampleNum];
    constraint[k] *= (real32_T)pts1[j];
    k += 3U;
    constraint[k - 2U] = -(real32_T)pts2[j + maxSampleNum];
    j++;
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void normPts_S16_R(const int16_T pts[], const uint32_T samples[], uint32_T
                   ptsNum, uint32_T sampleNum, real32_T ptsNorm[], real32_T
                   scale[], real32_T cents[])
{
  uint32_T i;
  uint32_T j;
  real32_T sumDis;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  /* Normalize the points. */
  cents[0U] = 0.0F;
  cents[1U] = 0.0F;
  for (i = 0U; i < sampleNum; i++) {
    j = samples[i];
    sumDis = pts[j + ptsNum];
    ptsNorm[i + sampleNum] = sumDis;
    cents[0U] += sumDis;
    sumDis = pts[j];
    ptsNorm[i] = sumDis;
    cents[1U] += sumDis;
  }
  cents[0U] /= (real32_T)sampleNum;
  cents[1U] /= (real32_T)sampleNum;
  sumDis = 0.0F;
  for (i = 0U; i < sampleNum; i++) {
    ptsNorm[i + sampleNum] -= cents[0U];
    ptsNorm[i] -= cents[1U];
    j = i + sampleNum;
    sumDis += sqrtf(ptsNorm[j] * ptsNorm[j] + ptsNorm[i] * ptsNorm[i]);
  }
  if (sumDis > 0.0F) {
    scale[0U] = 1.41421354F;
    scale[0U] *= (real32_T)sampleNum;
    scale[0U] /= sumDis;
    for (i = 0U; i < sampleNum + sampleNum; i++) {
      ptsNorm[i] *= scale[0U];
    }
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void makeConstraintMatrix_Projective_R_R(const real32_T pts1[], const real32_T
  pts2[], uint32_T sampleNum, uint32_T maxSampleNum, real32_T constraint[])
{
  uint32_T i;
  uint32_T j;
  uint32_T k;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  /* Generate the constraint matrix. */
  j = 0U;
  k = 1U;
  for (i = 0U; i < sampleNum; i++) {
    constraint[k - 1U] = 0.0F;
    constraint[k] = 0.0F;
    k += 2U;
    constraint[k - 1U] = 0.0F;
    constraint[k] = -pts1[j + maxSampleNum];
    k += 2U;
    constraint[k - 1U] = -pts1[j];
    constraint[k] = -1.0F;
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
    constraint[k] = 1.0F;
    k += 2U;
    constraint[k - 1U] = 0.0F;
    constraint[k] = 0.0F;
    k += 2U;
    constraint[k - 1U] = 0.0F;
    constraint[k] = -pts2[j + maxSampleNum];
    constraint[k] *= pts1[j + maxSampleNum];
    k++;
    constraint[k] = -pts2[j + maxSampleNum];
    constraint[k] *= pts1[j];
    k += 3U;
    constraint[k - 2U] = -pts2[j + maxSampleNum];
    j++;
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void QRV2Norm_real32_T(const real32_T V[], int32_T N, real32_T v2norm[])
{
  int32_T vi;
  int32_T i;
  real32_T Vd;
  real32_T tmp;
  real32_T val;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  val = 0.0F;
  vi = 0;
  for (i = 0; i < N; i++) {
    Vd = V[vi];
    if (fabsf(val) > fabsf(Vd)) {
      tmp = Vd / val;
      val = sqrtf(tmp * tmp + 1.0F) * fabsf(val);
    } else if (Vd == 0.0F) {
      val = 0.0F;
    } else {
      tmp = val / Vd;
      val = sqrtf(tmp * tmp + 1.0F) * fabsf(Vd);
    }
    vi++;
  }
  v2norm[0U] = val;
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void QRDC_real32_T(real32_T xVec[], real32_T qrAux[], int32_T jpvt[], real32_T
                   work[], int32_T M, int32_T N)
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
  real32_T t;
  real32_T nrmxl;
  real32_T maxnrm;
  real32_T tt;
  int32_T minVal;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  pl = 0;
  for (j = 0; j < N; j++) {
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
        for (i = M; i > 0; i--) {
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
  for (j = N - 1; j >= 0; j--) {
    if (jpvt[j] < 0) {
      jpvt[j] = -jpvt[j];
      if (j != pu) {
        /* Swap columns pu and j */
        px = pu * M;
        px2 = j * M;
        for (i = M; i > 0; i--) {
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
  for (j = pl; j <= pu; j++) {
    QRV2Norm_real32_T(&xVec[j * M], M, &maxnrm);
    qrAux[j] = maxnrm;
    work[j] = qrAux[j];
  }
  /* perform the Householder reduction of x */
  if (M < N) {
    minVal = M;
  } else {
    minVal = N;
  }
  for (l = 0; l < minVal; l++) {
    mml = M - l;
    /* locate the column of largest norm and bring it into the pivot position */
    if ((l >= pl) && (l < pu)) {
      maxnrm = 0.0F;
      maxj = l;
      for (j = l; j <= pu; j++) {
        if (qrAux[j] > maxnrm) {
          maxnrm = qrAux[j];
          maxj = j;
        }
      }
      if (maxj != l) {
        /* Swap columns l and maxj */
        px = l * M;
        px2 = maxj * M;
        for (i = M; i > 0; i--) {
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
    qrAux[l] = 0.0F;
    if (M - 1 != l) {
      /* Compute the Householder transformation for column l */
      maxj = (M + 1) * l;
      px = maxj;
      QRV2Norm_real32_T(&xVec[maxj], mml, &nrmxl);
      if (fabsf(nrmxl) != 0.0F) {
        if (fabsf(xVec[maxj]) != 0.0F) {
          if (xVec[maxj] >= 0.0F) {
            nrmxl = fabsf(nrmxl);
          } else {
            nrmxl = -fabsf(nrmxl);
          }
        }
        /* Check if it's safe to multiply by 1/nrmxl instead of dividing by nrmxl */
        px2 = maxj;
        if (5.0E-20F * nrmxl != 0.0F) {
          maxnrm = 1.0F / nrmxl;
          for (i = mml; i > 0; i--) {
            xVec[px2] *= maxnrm;
            px2++;
          }
        } else {
          for (i = mml; i > 0; i--) {
            xVec[px2] /= nrmxl;
            px2++;
          }
        }
        xVec[maxj]++;
        /* apply the transformation to the remaining columns, updating the norms. */
        for (j = l + 1; j < N; j++) {
          plj = j * M + l;
          px2 = plj;
          t = 0.0F;
          for (i = mml; i > 0; i--) {
            t -= xVec[px] * xVec[px2];
            px++;
            px2++;
          }
          px = maxj;
          px2 = plj;
          t /= xVec[maxj];
          for (i = mml; i > 0; i--) {
            xVec[px2] += t * xVec[px];
            px++;
            px2++;
          }
          px = maxj;
          px2 = 0;
          if (qrAux[j] == 0.0F) {
            px2 = 1;
          }
          if (!((j < pl) || (j > pu) || (px2 != 0))) {
            maxnrm = fabsf(xVec[plj]) / qrAux[j];
            tt = 1.0F - maxnrm * maxnrm;
            if (tt < 0.0F) {
              tt = 0.0F;
            }
            t = tt;
            maxnrm = qrAux[j] / work[j];
            tt = 0.05F * tt * maxnrm * maxnrm + 1.0F;
            if (tt != 1.0F) {
              qrAux[j] *= sqrtf(t);
            } else {
              QRV2Norm_real32_T(&xVec[plj + 1], mml - 1, &maxnrm);
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
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void QRCompQy_real32_T(real32_T qr[], const real32_T qrAuxj[], real32_T y[],
  int32_T n, int32_T j)
{
  real32_T j_0;
  int32_T nmj;
  int32_T i;
  int32_T pjj;
  real32_T t;
  int32_T pqr;
  int32_T py;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  if (fabsf(qrAuxj[0U]) != 0.0F) {
    nmj = n - j;
    pjj = (n + 1) * j;
    pqr = pjj;
    j_0 = qr[pjj];
    qr[pjj] = qrAuxj[0U];
    t = 0.0F;
    py = 0;
    for (i = nmj; i > 0; i--) {
      t -= y[py] * qr[pqr];
      pqr++;
      py++;
    }
    pqr = pjj;
    t /= qr[pjj];
    py = 0;
    for (i = nmj; i > 0; i--) {
      y[py] += t * qr[pqr];
      pqr++;
      py++;
    }
    qr[pjj] = j_0;
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void QRSL1_real32_T(real32_T qr[], const real32_T qrAux[], real32_T y[], int32_T
                    n, int32_T k)
{
  int32_T pqraux;
  int32_T py;
  int32_T u1;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  u1 = n - 1;
  if (k < u1) {
    u1 = k;
  }
  u1--;
  if (u1 + 1 != 0) {
    pqraux = u1;
    py = u1;
    while (u1 + 1 > 0) {
      QRCompQy_real32_T(qr, &qrAux[pqraux], &y[py], n, u1);
      pqraux--;
      py--;
      u1--;
    }
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
void QRE_real32_T(real32_T outQ[], real32_T outR[], real32_T outE[], real32_T
                  qrAux[], real32_T work[], int32_T jpvt[], real32_T sPtr[],
                  int32_T M, int32_T N, boolean_T economy)
{
  int32_T j;
  int32_T L;
  int32_T pq;
  int32_T ps;
  /* S-Function (svipesttform): '<S1>/est_geom_trans' */
  QRDC_real32_T(outR, qrAux, jpvt, work, M, N);
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
  for (j = 0; j < M * L; j++) {
    outQ[j] = 0.0F;
  }
  for (j = 0; j < L; j++) {
    outQ[pq] = 1.0F;
    pq = (pq + M) + 1;
  }
  /* Convert cols of identity into cols of q. Use info stored in lower triangle of r and in vector qraux to work on columns of identity matrix I and transform them into q*I(:,j) i.e. the columns of q. */
  pq = 0;
  for (j = 0; j < L; j++) {
    QRSL1_real32_T(outR, qrAux, &outQ[pq], M, N);
    pq += M;
  }
  if (economy && (M > N)) {
    /* Copy upper triangle of r to s */
    L = 0;
    ps = 0;
    for (j = 1; j - 1 < N; j++) {
      for (pq = 0; pq < j; pq++) {
        sPtr[ps] = outR[L];
        ps++;
        L++;
      }
      L = (L + M) - j;
      for (pq = 0; pq < N - j; pq++) {
        sPtr[ps] = 0.0F;
        ps++;
      }
    }
  } else {
    /* Zero strict lower triangle of r */
    L = M * N - 1;
    for (j = N; j - 1 >= 0; j--) {
      for (pq = M; pq > j; pq--) {
        outR[L] = 0.0F;
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
  for (j = 0; j < N; j++) {
    outE[L] = (real32_T)(jpvt[pq] + 1);
    L++;
    pq++;
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
}
/* Model step function */
void mdl_estgeomtrans0_380_hw(const int16_T inputArg1[300], const int16_T
  inputArg2[300], const uint16_T inputArg3, real32_T outputArg1[9])
{

DW_mdl_estgeomtrans0_380_T mdl_estgeomtrans0_380_DW;

static bool init=true;
if(init){
{
  /* Start for S-Function (svipesttform): '<S1>/est_geom_trans' */
  srand(clock());
}
  init=false;
}
  int32_T i;
  uint16_T est_geom_trans_DW_BEST_SAMPLE[150];
  uint32_T bestInlierNum;
  boolean_T mIsRandomSamplingStep;
  real32_T disThr;
  uint32_T loopCount;
  boolean_T isContinue;
  int32_T inlierNum;
  uint32_T i_0;
  uint32_T est_geom_trans_DW_SAMPLEIDX[150];
  boolean_T isDuplicate;
  boolean_T isOK;
  uint32_T bestCol;
  real32_T cents1[2];
  real32_T est_geom_trans_DW_PTSNORM1[300];
  real32_T scale1;
  real32_T cents2[2];
  real32_T est_geom_trans_DW_PTSNORM2[300];
  real32_T scale2;
  real32_T Constraint[72];
  int32_T JPVT[8];
  real32_T RV[81];
  real32_T Work[8];
  real32_T Qraux[8];
  real32_T E[8];
  real32_T Q[72];
  real32_T maxSum;
  uint32_T j;
  uint32_T m;
  real32_T E_0[9];
  real32_T tformCompact[9];
  real32_T s1DivS2;
  real32_T tformCompact_0[9];
  real32_T est_geom_trans_DW_DISTANCE[151];
  int32_T right;
  int32_T mid;
  int32_T jj;
  real32_T bestTFormCompact[9];
  real32_T rtb_est_geom_trans[9];
  /* S-Function (svipesttform): '<S1>/est_geom_trans' incorporates:
   *  Inport: '<Root>/In1'
   *  Inport: '<Root>/In2'
   *  Inport: '<Root>/In3'
   */
  for (i = 0; i < 9; i++) {
    rtb_est_geom_trans[i] = 0.0F;
  }
  if ((inputArg3 >= 4) && (inputArg3 <= 150)) {
    if (inputArg3 == 4) {
      est_geom_trans_DW_BEST_SAMPLE[0] = 0U;
      est_geom_trans_DW_BEST_SAMPLE[1] = 1U;
      est_geom_trans_DW_BEST_SAMPLE[2] = 2U;
      est_geom_trans_DW_BEST_SAMPLE[3] = 3U;
      bestInlierNum = 4U;
      mIsRandomSamplingStep = false;
    } else {
      bestInlierNum = 0U;
      mIsRandomSamplingStep = true;
    }
    disThr = 0.0F;
    loopCount = 0U;
    isContinue = true;
    inlierNum = 4;
    makeConstraintMatrix_Projective_S16_R((const int16_T *)&inputArg1[0], (const
      int16_T *)&inputArg2[0], (uint32_T)inputArg3, 150U,
      &mdl_estgeomtrans0_380_DW.est_geom_trans_DW_CONSTRT_ALL[0]);
    while (isContinue) {
      if (mIsRandomSamplingStep) {
        i_0 = 0U;
        while (i_0 < 4U) {
          j = 0U;
          while (j < 10U) {
            est_geom_trans_DW_SAMPLEIDX[i_0] = rand() % inputArg3;
            isDuplicate = false;
            m = 0U;
            while (m < i_0) {
              if (est_geom_trans_DW_SAMPLEIDX[m] ==
                  est_geom_trans_DW_SAMPLEIDX[i_0]) {
                isDuplicate = true;
                m = i_0;
              }
              m++;
            }
            if (!isDuplicate) {
              j = 10U;
            }
            j++;
          }
          if (isDuplicate) {
            isContinue = false;
            i_0 = 4U;
          }
          i_0++;
        }
      } else if (bestInlierNum >= 4U) {
        for (i = 0; i < (int32_T)bestInlierNum; i++) {
          est_geom_trans_DW_SAMPLEIDX[i] = est_geom_trans_DW_BEST_SAMPLE[i];
        }
        inlierNum = (int32_T)bestInlierNum;
      } else {
        isContinue = false;
      }
      if (isContinue) {
        loopCount++;
        bestCol = 0U;
        normPts_S16_R((const int16_T *)&inputArg1[0], (const uint32_T *)
                      &est_geom_trans_DW_SAMPLEIDX[0], 150U, (uint32_T)inlierNum,
                      &est_geom_trans_DW_PTSNORM1[0], &scale1, &cents1[0]);
        normPts_S16_R((const int16_T *)&inputArg2[0], (const uint32_T *)
                      &est_geom_trans_DW_SAMPLEIDX[0], 150U, (uint32_T)inlierNum,
                      &est_geom_trans_DW_PTSNORM2[0], &scale2, &cents2[0]);
        if (inlierNum == 4) {
          makeConstraintMatrix_Projective_R_R((const real32_T *)
            &est_geom_trans_DW_PTSNORM1[0], (const real32_T *)
            &est_geom_trans_DW_PTSNORM2[0], 4U, 4U, &Constraint[0]);
          for (i_0 = 0U; i_0 < 8U; i_0++) {
            JPVT[i_0] = 0;
          }
          QRE_real32_T(&Q[0], &Constraint[0], &E[0], &Qraux[0], &Work[0], &JPVT
                       [0], &RV[0], 9, 8, true);
          maxSum = 0.0F;
          for (i_0 = 0U; i_0 < 9U; i_0++) {
            for (j = 0U; j < 9U; j++) {
              if (i_0 != j) {
                s1DivS2 = 0.0F;
              } else {
                s1DivS2 = -1.0F;
              }
              for (m = 0U; m < 8U; m++) {
                s1DivS2 += Q[m * 9U + i_0] * Q[m * 9U + j];
              }
              RV[i_0 * 9U + j] = s1DivS2;
              if (!(s1DivS2 >= 0.0F)) {
                s1DivS2 = -s1DivS2;
              }
              if (maxSum < s1DivS2) {
                maxSum = s1DivS2;
                bestCol = i_0;
              }
            }
          }
          isOK = true;
        } else {
          makeConstraintMatrix_Projective_R_R((const real32_T *)
            &est_geom_trans_DW_PTSNORM1[0], (const real32_T *)
            &est_geom_trans_DW_PTSNORM2[0], (uint32_T)inlierNum, (uint32_T)
            inlierNum, &mdl_estgeomtrans0_380_DW.est_geom_trans_DW_FACTOR[0]);
          m = 0U;
          for (i_0 = 0U; i_0 < 9U; i_0++) {
            for (j = 0U; j < (uint32_T)(inlierNum + inlierNum); j++) {
              mdl_estgeomtrans0_380_DW.est_geom_trans_DW_CONSTRAINT[m] =
                mdl_estgeomtrans0_380_DW.est_geom_trans_DW_FACTOR[j * 9U + i_0];
              m++;
            }
          }
          i = MWVIP_SVD_R
            (&mdl_estgeomtrans0_380_DW.est_geom_trans_DW_CONSTRAINT[0],
             (uint32_T)(inlierNum << 1), 9, &est_geom_trans_DW_PTSNORM1[0],
             &E_0[0], &est_geom_trans_DW_PTSNORM2[0],
             &mdl_estgeomtrans0_380_DW.est_geom_trans_DW_FACTOR[0], &RV[0], 1);
          isOK = (i == 0);
          bestCol = 8U;
        }
        if (isOK) {
          j = bestCol * 9U;
          tformCompact[0] = RV[j];
          j++;
          for (i_0 = 0U; i_0 < 8U; i_0++) {
            tformCompact[(int32_T)i_0 + 1] = RV[j];
            j++;
          }
          maxSum = 1.0F / scale2;
          s1DivS2 = scale1 * maxSum;
          tformCompact_0[6] = tformCompact[6] * scale1;
          tformCompact_0[7] = tformCompact[7] * scale1;
          tformCompact_0[8] = (tformCompact[8] - tformCompact_0[6] * cents1[0])
            - tformCompact_0[7] * cents1[1];
          tformCompact_0[0] = tformCompact[0] * s1DivS2;
          tformCompact_0[1] = tformCompact[1] * s1DivS2;
          tformCompact_0[2] = ((tformCompact_0[8] * cents2[0] - tformCompact_0[0]
                                * cents1[0]) - tformCompact_0[1] * cents1[1]) +
            tformCompact[2] * maxSum;
          tformCompact_0[3] = tformCompact[3] * s1DivS2;
          tformCompact_0[4] = tformCompact[4] * s1DivS2;
          tformCompact_0[5] = ((tformCompact_0[8] * cents2[1] - tformCompact_0[3]
                                * cents1[0]) - tformCompact_0[4] * cents1[1]) +
            tformCompact[5] * maxSum;
          tformCompact_0[0] += tformCompact_0[6] * cents2[0];
          tformCompact_0[1] += tformCompact_0[7] * cents2[0];
          tformCompact_0[3] += tformCompact_0[6] * cents2[1];
          tformCompact_0[4] += tformCompact_0[7] * cents2[1];
          if (tformCompact_0[8U] != 0.0F) {
            maxSum = 1.0F / tformCompact_0[8];
            for (i_0 = 0U; i_0 < 8U; i_0++) {
              tformCompact_0[i_0] *= maxSum;
            }
            tformCompact_0[8] = 1.0F;
          } else {
            isOK = false;
          }
        }
        if (isOK) {
          bestCol = 0U;
          for (i_0 = 0U; i_0 < inputArg3; i_0++) {
            est_geom_trans_DW_DISTANCE[i_0] = 0.0F;
            for (j = 0U; j < 2U; j++) {
              maxSum =
                mdl_estgeomtrans0_380_DW.est_geom_trans_DW_CONSTRT_ALL[bestCol] *
                tformCompact_0[0];
              bestCol++;
              for (m = 0U; m < 8U; m++) {
                maxSum +=
                  mdl_estgeomtrans0_380_DW.est_geom_trans_DW_CONSTRT_ALL[bestCol]
                  * tformCompact_0[(int32_T)m + 1];
                bestCol++;
              }
              est_geom_trans_DW_DISTANCE[i_0] += maxSum * maxSum;
            }
          }
          for (i = 0; i < inputArg3; i++) {
            est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_DISTANCE[i];
          }
          i_0 = (uint32_T)(inputArg3 >> 1);
          i = 0;
          right = (int32_T)(inputArg3 - 1U);
          while (right > i + 1) {
            mid = (i + right) >> 1;
            if (est_geom_trans_DW_PTSNORM1[i] > est_geom_trans_DW_PTSNORM1[mid])
            {
              s1DivS2 = est_geom_trans_DW_PTSNORM1[i];
              est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_PTSNORM1[mid];
              est_geom_trans_DW_PTSNORM1[mid] = s1DivS2;
            }
            if (est_geom_trans_DW_PTSNORM1[i] > est_geom_trans_DW_PTSNORM1[right])
            {
              s1DivS2 = est_geom_trans_DW_PTSNORM1[i];
              est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_PTSNORM1[right];
              est_geom_trans_DW_PTSNORM1[right] = s1DivS2;
            }
            if (est_geom_trans_DW_PTSNORM1[mid] >
                est_geom_trans_DW_PTSNORM1[right]) {
              s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
              est_geom_trans_DW_PTSNORM1[mid] = est_geom_trans_DW_PTSNORM1[right];
              est_geom_trans_DW_PTSNORM1[right] = s1DivS2;
            }
            s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
            est_geom_trans_DW_PTSNORM1[mid] = est_geom_trans_DW_PTSNORM1[right -
              1];
            est_geom_trans_DW_PTSNORM1[right - 1] = s1DivS2;
            est_geom_trans_DW_PTSNORM1[(uint32_T)inputArg3] =
              est_geom_trans_DW_PTSNORM1[right - 1];
            mid = i;
            jj = right - 1;
            isContinue = true;
            while (isContinue) {
              for (mid++; est_geom_trans_DW_PTSNORM1[mid] <
                   est_geom_trans_DW_PTSNORM1[(uint32_T)inputArg3]; mid++) {
              }
              if (mid > (int32_T)(inputArg3 - 1U)) {
                mid = (int32_T)(inputArg3 - 1U);
              }
              for (jj--; est_geom_trans_DW_PTSNORM1[jj] >
                   est_geom_trans_DW_PTSNORM1[(uint32_T)inputArg3]; jj--) {
              }
              if (jj < 0) {
                jj = 0;
              }
              if (mid >= jj) {
                isContinue = false;
              } else {
                s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
                est_geom_trans_DW_PTSNORM1[mid] = est_geom_trans_DW_PTSNORM1[jj];
                est_geom_trans_DW_PTSNORM1[jj] = s1DivS2;
              }
            }
            s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
            est_geom_trans_DW_PTSNORM1[mid] = est_geom_trans_DW_PTSNORM1[right -
              1];
            est_geom_trans_DW_PTSNORM1[right - 1] = s1DivS2;
            if (mid >= (int32_T)i_0) {
              right = mid - 1;
            }
            if (mid <= (int32_T)i_0) {
              i = mid + 1;
            }
          }
          if ((right - i == 1) && (est_geom_trans_DW_PTSNORM1[i] >
               est_geom_trans_DW_PTSNORM1[right])) {
            s1DivS2 = est_geom_trans_DW_PTSNORM1[i];
            est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_PTSNORM1[right];
            est_geom_trans_DW_PTSNORM1[right] = s1DivS2;
          }
          maxSum = est_geom_trans_DW_PTSNORM1[i_0];
          if (!((inputArg3 & 1U) != 0U)) {
            for (i = 0; i < inputArg3; i++) {
              est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_DISTANCE[i];
            }
            i_0--;
            i = 0;
            right = (int32_T)(inputArg3 - 1U);
            while (right > i + 1) {
              mid = (i + right) >> 1;
              if (est_geom_trans_DW_PTSNORM1[i] > est_geom_trans_DW_PTSNORM1[mid])
              {
                s1DivS2 = est_geom_trans_DW_PTSNORM1[i];
                est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_PTSNORM1[mid];
                est_geom_trans_DW_PTSNORM1[mid] = s1DivS2;
              }
              if (est_geom_trans_DW_PTSNORM1[i] >
                  est_geom_trans_DW_PTSNORM1[right]) {
                s1DivS2 = est_geom_trans_DW_PTSNORM1[i];
                est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_PTSNORM1[right];
                est_geom_trans_DW_PTSNORM1[right] = s1DivS2;
              }
              if (est_geom_trans_DW_PTSNORM1[mid] >
                  est_geom_trans_DW_PTSNORM1[right]) {
                s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
                est_geom_trans_DW_PTSNORM1[mid] =
                  est_geom_trans_DW_PTSNORM1[right];
                est_geom_trans_DW_PTSNORM1[right] = s1DivS2;
              }
              s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
              est_geom_trans_DW_PTSNORM1[mid] = est_geom_trans_DW_PTSNORM1[right
                - 1];
              est_geom_trans_DW_PTSNORM1[right - 1] = s1DivS2;
              est_geom_trans_DW_PTSNORM1[(uint32_T)inputArg3] =
                est_geom_trans_DW_PTSNORM1[right - 1];
              mid = i;
              jj = right - 1;
              isContinue = true;
              while (isContinue) {
                for (mid++; est_geom_trans_DW_PTSNORM1[mid] <
                     est_geom_trans_DW_PTSNORM1[(uint32_T)inputArg3]; mid++) {
                }
                if (mid > (int32_T)(inputArg3 - 1U)) {
                  mid = (int32_T)(inputArg3 - 1U);
                }
                for (jj--; est_geom_trans_DW_PTSNORM1[jj] >
                     est_geom_trans_DW_PTSNORM1[(uint32_T)inputArg3]; jj--) {
                }
                if (jj < 0) {
                  jj = 0;
                }
                if (mid >= jj) {
                  isContinue = false;
                } else {
                  s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
                  est_geom_trans_DW_PTSNORM1[mid] =
                    est_geom_trans_DW_PTSNORM1[jj];
                  est_geom_trans_DW_PTSNORM1[jj] = s1DivS2;
                }
              }
              s1DivS2 = est_geom_trans_DW_PTSNORM1[mid];
              est_geom_trans_DW_PTSNORM1[mid] = est_geom_trans_DW_PTSNORM1[right
                - 1];
              est_geom_trans_DW_PTSNORM1[right - 1] = s1DivS2;
              if (mid >= (int32_T)i_0) {
                right = mid - 1;
              }
              if (mid <= (int32_T)i_0) {
                i = mid + 1;
              }
            }
            if ((right - i == 1) && (est_geom_trans_DW_PTSNORM1[i] >
                 est_geom_trans_DW_PTSNORM1[right])) {
              s1DivS2 = est_geom_trans_DW_PTSNORM1[i];
              est_geom_trans_DW_PTSNORM1[i] = est_geom_trans_DW_PTSNORM1[right];
              est_geom_trans_DW_PTSNORM1[right] = s1DivS2;
            }
            s1DivS2 = est_geom_trans_DW_PTSNORM1[i_0];
            maxSum = (maxSum + s1DivS2) * 0.5F;
          }
          if ((maxSum < disThr) || (loopCount == 1U)) {
            disThr = maxSum;
            for (i = 0; i < 9; i++) {
              bestTFormCompact[i] = tformCompact_0[i];
            }
            bestInlierNum = 0U;
            for (i = 0; i < inputArg3; i++) {
              if (est_geom_trans_DW_DISTANCE[i] <= maxSum) {
                est_geom_trans_DW_BEST_SAMPLE[bestInlierNum] = (uint16_T)i;
                bestInlierNum++;
              }
            }
          }
          if (mIsRandomSamplingStep) {
            if (loopCount >= 1000U) {
              mIsRandomSamplingStep = false;
              loopCount = 0U;
            }
            isContinue = true;
          } else {
            isContinue = false;
          }
        } else if (!mIsRandomSamplingStep) {
          isContinue = false;
          bestInlierNum = 0U;
        } else {
          if (loopCount >= 1000U) {
            isContinue = false;
          }
        }
      }
    }
    if (bestInlierNum >= 4U) {
      rtb_est_geom_trans[0] = bestTFormCompact[4];
      rtb_est_geom_trans[1] = bestTFormCompact[3];
      rtb_est_geom_trans[2] = bestTFormCompact[5];
      rtb_est_geom_trans[3] = bestTFormCompact[1];
      rtb_est_geom_trans[4] = bestTFormCompact[0];
      rtb_est_geom_trans[5] = bestTFormCompact[2];
      rtb_est_geom_trans[6] = bestTFormCompact[7];
      rtb_est_geom_trans[7] = bestTFormCompact[6];
      rtb_est_geom_trans[8] = bestTFormCompact[8];
    }
  }
  /* End of S-Function (svipesttform): '<S1>/est_geom_trans' */
  /* Outport: '<Root>/Out1' */
  for (i = 0; i < 9; i++) {
    outputArg1[i] = rtb_est_geom_trans[i];
  }
  /* End of Outport: '<Root>/Out1' */
}
/* Model initialize function */
/* Model terminate function */
/*
 * File trailer for generated code.
 *
 * [EOF]
 */
#include <stdio.h>
int inputArg1 input("inputArg1");
int inputArg2 input("inputArg2");
int inputArg3 input("inputArg3");
int outputArg1 output("outputArg1");
void main() {

	int16_T input_1[300];
	int16_T input_2[300];
	uint16_T input_3[1];
	float output_1[9];

	while(1) {
		for(int i = 0; i < 300; ++i) {
			input_1[i] =fgetc(inputArg1);
		}
		for(int i = 0; i < 300; ++i) {
			input_2[i] =fgetc(inputArg2);
		}
		for(int i = 0; i < 1; ++i) {
			input_3[i] =fgetc(inputArg3);
		}

		mdl_estgeomtrans0_380_hw( input_1, input_2, input_3[0], output_1 );

		for(int i = 0; i < 9; ++i) {
			fput_float(output_1[i], outputArg1);
		}
	}
}
