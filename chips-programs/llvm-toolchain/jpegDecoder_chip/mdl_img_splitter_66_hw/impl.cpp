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
 * File: mdl_img_splitter_66.c
 *
 * Code generated for Simulink model 'mdl_img_splitter_66'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Mon Oct  8 18:58:28 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_img_splitter_66_hw.hpp"
/* Model step function */
void mdl_img_splitter_66_hw(const uint32_T inputArg1[45], uint32_T outputArg1[15],
  uint32_T outputArg2[15], uint32_T outputArg3[15])
{


static bool init=true;
if(init){
{
  /* (no initialization code required) */
}
  init=false;
}
  int32_T i;
  /* MATLAB Function 'img_splitter/img_split': '<S2>:1' */
  /* '<S2>:1:3' r = rgbImage(:,:,1); */
  /*  Red channel */
  /* '<S2>:1:4' g = rgbImage(:,:,2); */
  /*  Green channel */
  /* '<S2>:1:5' b = rgbImage(:,:,3); */
  /*  Blue channel */
  for (i = 0; i < 15; i++) {
    /* Outport: '<Root>/Out1' incorporates:
     *  Inport: '<Root>/In1'
     *  MATLAB Function: '<S1>/img_split'
     */
    outputArg1[i] = inputArg1[i];
    /* Outport: '<Root>/Out2' incorporates:
     *  Inport: '<Root>/In1'
     *  MATLAB Function: '<S1>/img_split'
     */
    outputArg2[i] = inputArg1[i + 15];
    /* Outport: '<Root>/Out3' incorporates:
     *  Inport: '<Root>/In1'
     *  MATLAB Function: '<S1>/img_split'
     */
    outputArg3[i] = inputArg1[i + 30];
  }
}
