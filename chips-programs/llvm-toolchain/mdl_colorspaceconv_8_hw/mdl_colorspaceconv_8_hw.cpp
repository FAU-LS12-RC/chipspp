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
 * File: mdl_colorspaceconv_8.c
 *
 * Code generated for Simulink model 'mdl_colorspaceconv_8'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Wed May 23 14:41:00 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_colorspaceconv_8_hw.hpp"

/* Block states (auto storage) */
/* Model step function */
void mdl_colorspaceconv_8_hw(const uint8_T inputArg1[64], const uint8_T
  inputArg2[64], const uint8_T inputArg3[64], uint8_T rtb_out_frame[192])
{

DW_mdl_colorspaceconv_8_T mdl_colorspaceconv_8_DW;

static bool init=true;
#pragma HLS reset variable=init
#pragma HLS inline region recursive
#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis port=inputArg1
#pragma HLS INTERFACE axis port=inputArg2
#pragma HLS INTERFACE axis port=inputArg3
#pragma HLS INTERFACE axis port=outputArg1
if(init){
{
  /* (no initialization code required) */
}
  init=false;
}
  int32_T j;
  uint32_T cc1;
  uint32_T pos;
  uint32_T neg;
  uint32_T yDiff;
  int32_T iy;
  //uint8_T rtb_out_frame[192];
  uint8_T ColorSpaceConversion_o3[64];
  uint8_T ColorSpaceConversion_o2[64];
  /* MATLAB Function: '<S2>/img_merger' */
  /* Precompute constants */
  /*  Recombine separate color channels into an RGB image. */
  /* MATLAB Function 'colorspaceconv/img_merger/img_merger': '<S4>:1' */
  /* '<S4>:1:4' out_frame = cat(3, r, g, b); */
  iy = -1;
  for (j = 0; j < 64; j++) {
    /* S-Function (svipcolorconv): '<S1>/Color Space  Conversion' incorporates:
     *  Inport: '<Root>/In1'
     *  Inport: '<Root>/In2'
     *  Inport: '<Root>/In3'
     */
    /* Convert YcbCr to RGB; apply coefficients and offsets */
    /* derived from the ITU BT.601-5 recommendation; all of the */
    /* coefficients and offsets are scaled (by 2^14) such that */
    /* the conversion can be done using integer arithmetic; this */
    /* routine relies on the user supplying the data in proper */
    /* ranges: Y [16..235], Cb & Cr [16..240] */
    /* Note that all of the operations are grouped in such a way */
    /* that the arithmetic can be done using unsigned integers */
    /* Given that Y is in the proper range, yDiff should */
    /* always be positive */
    yDiff = inputArg1[j] - 16U;
    /* Red; 8192 in this equations is 0.5*2^14 or 2^13; adding 0.5  */
    /* before truncation will result in rounding */
    pos = (yDiff * 19077U + inputArg3[j] * 26149U) + 8192U;
    if (pos > 3347072U) {
      pos -= 3347072U;
    } else {
      pos = 0U;
    }
    cc1 = pos >> 14;
    /* limit to avoid wrapping */
    if (cc1 > 255U) {
      cc1 = 255U;
    }
    /* Compute green channel */
    pos = yDiff * 19077U + 2534784U;
    neg = inputArg2[j] * 6419U + inputArg3[j] * 13320U;
    /* scale back */
    if (pos > neg) {
      pos -= neg;
    } else {
      pos = 0U;
    }
    neg = pos >> 14;
    if (neg > 255U) {
      neg = 255U;
    }
    /* Compute blue channel */
    pos = (yDiff * 19077U + inputArg2[j] * 33050U) + 8192U;
    /* scale back */
    if (pos > 4230400U) {
      pos -= 4230400U;
    } else {
      pos = 0U;
    }
    yDiff = pos >> 14;
    if (yDiff > 255U) {
      yDiff = 255U;
    }
    /* assign back the results */
    ColorSpaceConversion_o2[j] = (uint8_T)neg;
    ColorSpaceConversion_o3[j] = (uint8_T)yDiff;
    /* MATLAB Function: '<S2>/img_merger' incorporates:
     *  S-Function (svipcolorconv): '<S1>/Color Space  Conversion'
     */
    iy++;
    rtb_out_frame[iy] = (uint8_T)cc1;
#pragma HLS pipeline
  }
  /* MATLAB Function: '<S2>/img_merger' */
  iy++;
  rtb_out_frame[iy] = ColorSpaceConversion_o2[0];
  for (j = 0; j < 63; j++) {
    iy++;
    rtb_out_frame[iy] = ColorSpaceConversion_o2[j + 1];
#pragma HLS pipeline
  }
  iy++;
  rtb_out_frame[iy] = ColorSpaceConversion_o3[0];
  for (j = 0; j < 63; j++) {
    iy++;
    rtb_out_frame[iy] = ColorSpaceConversion_o3[j + 1];
#pragma HLS pipeline
  }
  /* MATLAB Function: '<S3>/frame_buffer ' */
  /* MATLAB Function 'frame_buffer/frame_buffer ': '<S5>:1' */
  /*  */
  /* -------------------------------------------------------------------------- */
  /*  Copyright (c) 2017 Hardware-Software-Co-Design, Friedrich- */
  /*  Alexander-Universitaet Erlangen-Nuernberg (FAU), Germany. */
  /*  All rights reserved. */
  /*  */
  /*  This code and any associated documentation is provided "as is" */
  /*  */
  /*  IN NO EVENT SHALL HARDWARE-SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER- */
  /*  UNIVERSITAET ERLANGEN-NUERNBERG (FAU) BE LIABLE TO ANY PARTY FOR DIRECT, */
  /*  INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT */
  /*  OF THE USE OF THIS CODE AND ITS DOCUMENTATION, EVEN IF HARDWARE- */
  /*  SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET ERLANGEN-NUERNBERG */
  /*  (FAU) HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. THE */
  /*  AFOREMENTIONED EXCLUSIONS OF LIABILITY DO NOT APPLY IN CASE OF INTENT */
  /*  BY HARDWARE-SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET */
  /*  ERLANGEN-NUERNBERG (FAU). */
  /*  */
  /*  HARDWARE-SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET ERLANGEN- */
  /*  NUERNBERG (FAU), SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT */
  /*  NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS */
  /*  FOR A PARTICULAR PURPOSE. */
  /*  */
  /*  THE CODE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND HARDWARE- */
  /*  SOFTWARE-CO-DESIGN, FRIEDRICH-ALEXANDER-UNIVERSITAET ERLANGEN- */
  /*  NUERNBERG (FAU) HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, */
  /*  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. */
  /*  ------------------------------------------------------------------------- */
  /*  */
  /*   @author Streit Franz-Josef  */
  /*   @date   23 April 2018 */
  /*   @version 0.1 */
  /*   @brief This function creates memory in HW for non sequentiell access by  */
  /*          the AXI-Stream interface */
  /*  */
  /* '<S5>:1:42' tmp=cast(zeros(size(in_frame)),class(in_frame)); */
  /* '<S5>:1:44' for i=1:numel(in_frame) */
//  for (iy = 0; iy < 192; iy++) {
//    /* '<S5>:1:45' tmp(i) = in_frame(i); */
//    outputArg1[iy] = rtb_out_frame[iy];
//#pragma HLS pipeline
//  }
  /* End of MATLAB Function: '<S3>/frame_buffer ' */
  /* '<S5>:1:47' frame_buffer = tmp; */
}
/* Model initialize function */
/* Model terminate function */
/*
 * File trailer for generated code.
 *
 * [EOF]
 */

int input_1 input("input_1");
int input_2 input("input_2");
int input_3 input("input_3");
int output_1 output("output_1");
void main() {
  uint8_T inputArg1[64];
  uint8_T inputArg2[64];
  uint8_T inputArg3[64];
  uint8_T outputArg1[192];

  for(int i = 0; i < 64; ++i) {
    inputArg1[i] = fgetc(input_1);
    inputArg2[i] = fgetc(input_2);
    inputArg3[i] = fgetc(input_3);
  }
  mdl_colorspaceconv_8_hw(inputArg1, inputArg2, inputArg3,
      outputArg1);
  for(int i = 0; i < 192; ++i)
    fputc(outputArg1[i], output_1);
}
