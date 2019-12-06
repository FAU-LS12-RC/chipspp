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
 * File: mdl_estgeomtrans0_380.h
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

#ifndef RTW_HEADER_mdl_estgeomtrans0_380_h_
#define RTW_HEADER_mdl_estgeomtrans0_380_h_
#include <math.h>
#ifndef mdl_estgeomtrans0_380_COMMON_INCLUDES_
# define mdl_estgeomtrans0_380_COMMON_INCLUDES_
#include <stdlib.h>
#include <time.h>
#include "data_types.hpp"
#include "vipsvd_rt.h"
#endif                                 /* mdl_estgeomtrans0_380_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */

typedef struct {
  real32_T est_geom_trans_DW_CONSTRT_ALL[2700];/* '<S1>/est_geom_trans' */
  real32_T est_geom_trans_DW_FACTOR[2700];/* '<S1>/est_geom_trans' */
  real32_T est_geom_trans_DW_CONSTRAINT[2700];/* '<S1>/est_geom_trans' */
} DW_mdl_estgeomtrans0_380_T;



/* Customized model step function */
extern void mdl_estgeomtrans0_380_hw(const int16_T inputArg1[300], const int16_T
  inputArg2[300], const uint16_T inputArg3, real32_T outputArg1[9]);

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
 * '<Root>' : 'mdl_estgeomtrans0_380'
 * '<S1>'   : 'mdl_estgeomtrans0_380/EstGeomTrans0'
 */
#endif                                 /* RTW_HEADER_mdl_estgeomtrans0_380_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
