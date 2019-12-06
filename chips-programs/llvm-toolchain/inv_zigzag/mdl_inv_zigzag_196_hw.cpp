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
 * File: mdl_inv_zigzag_196.c
 *
 * Code generated for Simulink model 'mdl_inv_zigzag_196'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Wed May 23 14:41:31 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_inv_zigzag_196_hw.hpp"
/* Model step function */
void mdl_inv_zigzag_196_hw(int16_T rtb_frame_buffer_p[64], int16_T rtb_TmpSignalConversionAtSFunctionInport1[64])
{


static bool init=true;
#pragma HLS reset variable=init
#pragma HLS inline region recursive
#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis port=inputArg1
#pragma HLS INTERFACE axis port=outputArg1
if(init){
{
  /* (no initialization code required) */
}
  init=false;
}
  int32_T i;
//  int16_T rtb_frame_buffer_p[64];
//  int16_T rtb_TmpSignalConversionAtSFunctionInport1[64];
  /* MATLAB Function: '<S2>/frame_buffer ' incorporates:
   *  Inport: '<Root>/In1'
   */
  /* MATLAB Function 'frame_buffer/frame_buffer ': '<S4>:1' */
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
  /* '<S4>:1:42' tmp=cast(zeros(size(in_frame)),class(in_frame)); */
  /* '<S4>:1:44' for i=1:numel(in_frame) */
//  for (i = 0; i < 64; i++) {
//    /* '<S4>:1:45' tmp(i) = in_frame(i); */
//    rtb_frame_buffer_p[i] = inputArg1[i];
//#pragma HLS pipeline
//  }
  /* End of MATLAB Function: '<S2>/frame_buffer ' */
  /* SignalConversion: '<S5>/TmpSignal ConversionAt SFunction Inport1' incorporates:
   *  MATLAB Function: '<S3>/frame_buffer '
   */
  /* '<S4>:1:47' frame_buffer = tmp; */
  rtb_TmpSignalConversionAtSFunctionInport1[0] = rtb_frame_buffer_p[0];
  rtb_TmpSignalConversionAtSFunctionInport1[7] = rtb_frame_buffer_p[35];
  rtb_TmpSignalConversionAtSFunctionInport1[8] = rtb_frame_buffer_p[1];
  rtb_TmpSignalConversionAtSFunctionInport1[9] = rtb_frame_buffer_p[4];
  rtb_TmpSignalConversionAtSFunctionInport1[10] = rtb_frame_buffer_p[8];
  rtb_TmpSignalConversionAtSFunctionInport1[11] = rtb_frame_buffer_p[11];
  rtb_TmpSignalConversionAtSFunctionInport1[12] = rtb_frame_buffer_p[19];
  rtb_TmpSignalConversionAtSFunctionInport1[13] = rtb_frame_buffer_p[22];
  rtb_TmpSignalConversionAtSFunctionInport1[14] = rtb_frame_buffer_p[34];
  rtb_TmpSignalConversionAtSFunctionInport1[15] = rtb_frame_buffer_p[36];
  rtb_TmpSignalConversionAtSFunctionInport1[16] = rtb_frame_buffer_p[5];
  rtb_TmpSignalConversionAtSFunctionInport1[17] = rtb_frame_buffer_p[7];
  rtb_TmpSignalConversionAtSFunctionInport1[18] = rtb_frame_buffer_p[12];
  rtb_TmpSignalConversionAtSFunctionInport1[19] = rtb_frame_buffer_p[18];
  rtb_TmpSignalConversionAtSFunctionInport1[20] = rtb_frame_buffer_p[23];
  rtb_TmpSignalConversionAtSFunctionInport1[21] = rtb_frame_buffer_p[33];
  rtb_TmpSignalConversionAtSFunctionInport1[22] = rtb_frame_buffer_p[37];
  rtb_TmpSignalConversionAtSFunctionInport1[23] = rtb_frame_buffer_p[48];
  rtb_TmpSignalConversionAtSFunctionInport1[24] = rtb_frame_buffer_p[6];
  rtb_TmpSignalConversionAtSFunctionInport1[25] = rtb_frame_buffer_p[13];
  rtb_TmpSignalConversionAtSFunctionInport1[26] = rtb_frame_buffer_p[17];
  rtb_TmpSignalConversionAtSFunctionInport1[27] = rtb_frame_buffer_p[24];
  rtb_TmpSignalConversionAtSFunctionInport1[28] = rtb_frame_buffer_p[32];
  rtb_TmpSignalConversionAtSFunctionInport1[29] = rtb_frame_buffer_p[38];
  rtb_TmpSignalConversionAtSFunctionInport1[30] = rtb_frame_buffer_p[47];
  rtb_TmpSignalConversionAtSFunctionInport1[31] = rtb_frame_buffer_p[49];
  rtb_TmpSignalConversionAtSFunctionInport1[32] = rtb_frame_buffer_p[14];
  rtb_TmpSignalConversionAtSFunctionInport1[33] = rtb_frame_buffer_p[16];
  rtb_TmpSignalConversionAtSFunctionInport1[34] = rtb_frame_buffer_p[25];
  rtb_TmpSignalConversionAtSFunctionInport1[35] = rtb_frame_buffer_p[31];
  rtb_TmpSignalConversionAtSFunctionInport1[36] = rtb_frame_buffer_p[39];
  rtb_TmpSignalConversionAtSFunctionInport1[37] = rtb_frame_buffer_p[46];
  rtb_TmpSignalConversionAtSFunctionInport1[38] = rtb_frame_buffer_p[50];
  rtb_TmpSignalConversionAtSFunctionInport1[39] = rtb_frame_buffer_p[57];
  rtb_TmpSignalConversionAtSFunctionInport1[40] = rtb_frame_buffer_p[15];
  rtb_TmpSignalConversionAtSFunctionInport1[41] = rtb_frame_buffer_p[26];
  rtb_TmpSignalConversionAtSFunctionInport1[42] = rtb_frame_buffer_p[30];
  rtb_TmpSignalConversionAtSFunctionInport1[43] = rtb_frame_buffer_p[40];
  rtb_TmpSignalConversionAtSFunctionInport1[44] = rtb_frame_buffer_p[45];
  rtb_TmpSignalConversionAtSFunctionInport1[45] = rtb_frame_buffer_p[51];
  rtb_TmpSignalConversionAtSFunctionInport1[46] = rtb_frame_buffer_p[56];
  rtb_TmpSignalConversionAtSFunctionInport1[47] = rtb_frame_buffer_p[58];
  rtb_TmpSignalConversionAtSFunctionInport1[48] = rtb_frame_buffer_p[27];
  rtb_TmpSignalConversionAtSFunctionInport1[49] = rtb_frame_buffer_p[29];
  rtb_TmpSignalConversionAtSFunctionInport1[50] = rtb_frame_buffer_p[41];
  rtb_TmpSignalConversionAtSFunctionInport1[51] = rtb_frame_buffer_p[44];
  rtb_TmpSignalConversionAtSFunctionInport1[52] = rtb_frame_buffer_p[52];
  rtb_TmpSignalConversionAtSFunctionInport1[53] = rtb_frame_buffer_p[55];
  rtb_TmpSignalConversionAtSFunctionInport1[54] = rtb_frame_buffer_p[59];
  rtb_TmpSignalConversionAtSFunctionInport1[55] = rtb_frame_buffer_p[62];
  rtb_TmpSignalConversionAtSFunctionInport1[56] = rtb_frame_buffer_p[28];
  rtb_TmpSignalConversionAtSFunctionInport1[1] = rtb_frame_buffer_p[2];
  rtb_TmpSignalConversionAtSFunctionInport1[3] = rtb_frame_buffer_p[9];
  rtb_TmpSignalConversionAtSFunctionInport1[5] = rtb_frame_buffer_p[20];
  rtb_TmpSignalConversionAtSFunctionInport1[57] = rtb_frame_buffer_p[42];
  rtb_TmpSignalConversionAtSFunctionInport1[59] = rtb_frame_buffer_p[53];
  rtb_TmpSignalConversionAtSFunctionInport1[61] = rtb_frame_buffer_p[60];
  rtb_TmpSignalConversionAtSFunctionInport1[2] = rtb_frame_buffer_p[3];
  rtb_TmpSignalConversionAtSFunctionInport1[4] = rtb_frame_buffer_p[10];
  rtb_TmpSignalConversionAtSFunctionInport1[6] = rtb_frame_buffer_p[21];
  rtb_TmpSignalConversionAtSFunctionInport1[58] = rtb_frame_buffer_p[43];
  rtb_TmpSignalConversionAtSFunctionInport1[60] = rtb_frame_buffer_p[54];
  rtb_TmpSignalConversionAtSFunctionInport1[62] = rtb_frame_buffer_p[61];
  rtb_TmpSignalConversionAtSFunctionInport1[63] = rtb_frame_buffer_p[63];
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
//  for (i = 0; i < 64; i++) {
//    /* '<S5>:1:45' tmp(i) = in_frame(i); */
//    outputArg1[i] = rtb_TmpSignalConversionAtSFunctionInport1[i];
//#pragma HLS pipeline
//  }
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
int output_1 output("output_1");
void main() {
  int16_T inputArg1[64];
  int16_T outputArg1[64];

  for(int i = 0; i < 64; ++i) {
    inputArg1[i] = fgetc(input_1);
  }
  mdl_inv_zigzag_196_hw(inputArg1, outputArg1);
  for(int i = 0; i < 64; ++i)
    fputc(outputArg1[i], output_1);
}
