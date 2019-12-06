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
 * File: mdl_inv_zigzag_195.c
 *
 * Code generated for Simulink model 'mdl_inv_zigzag_195'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Mon Oct  1 17:42:15 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_inv_zigzag_195_hw.hpp"
/* Model step function */
void mdl_inv_zigzag_195_hw(const int16_T inputArg1[64], int16_T outputArg1[64])
{


static bool init=true;
if(init){
{
  /* (no initialization code required) */
}
  init=false;
}
  /* Outport: '<Root>/Out1' incorporates:
   *  Inport: '<Root>/In1'
   */
  outputArg1[0] = inputArg1[0];
  outputArg1[7] = inputArg1[35];
  outputArg1[8] = inputArg1[1];
  outputArg1[9] = inputArg1[4];
  outputArg1[10] = inputArg1[8];
  outputArg1[11] = inputArg1[11];
  outputArg1[12] = inputArg1[19];
  outputArg1[13] = inputArg1[22];
  outputArg1[14] = inputArg1[34];
  outputArg1[15] = inputArg1[36];
  outputArg1[16] = inputArg1[5];
  outputArg1[17] = inputArg1[7];
  outputArg1[18] = inputArg1[12];
  outputArg1[19] = inputArg1[18];
  outputArg1[20] = inputArg1[23];
  outputArg1[21] = inputArg1[33];
  outputArg1[22] = inputArg1[37];
  outputArg1[23] = inputArg1[48];
  outputArg1[24] = inputArg1[6];
  outputArg1[25] = inputArg1[13];
  outputArg1[26] = inputArg1[17];
  outputArg1[27] = inputArg1[24];
  outputArg1[28] = inputArg1[32];
  outputArg1[29] = inputArg1[38];
  outputArg1[30] = inputArg1[47];
  outputArg1[31] = inputArg1[49];
  outputArg1[32] = inputArg1[14];
  outputArg1[33] = inputArg1[16];
  outputArg1[34] = inputArg1[25];
  outputArg1[35] = inputArg1[31];
  outputArg1[36] = inputArg1[39];
  outputArg1[37] = inputArg1[46];
  outputArg1[38] = inputArg1[50];
  outputArg1[39] = inputArg1[57];
  outputArg1[40] = inputArg1[15];
  outputArg1[41] = inputArg1[26];
  outputArg1[42] = inputArg1[30];
  outputArg1[43] = inputArg1[40];
  outputArg1[44] = inputArg1[45];
  outputArg1[45] = inputArg1[51];
  outputArg1[46] = inputArg1[56];
  outputArg1[47] = inputArg1[58];
  outputArg1[48] = inputArg1[27];
  outputArg1[49] = inputArg1[29];
  outputArg1[50] = inputArg1[41];
  outputArg1[51] = inputArg1[44];
  outputArg1[52] = inputArg1[52];
  outputArg1[53] = inputArg1[55];
  outputArg1[54] = inputArg1[59];
  outputArg1[55] = inputArg1[62];
  outputArg1[56] = inputArg1[28];
  outputArg1[1] = inputArg1[2];
  outputArg1[3] = inputArg1[9];
  outputArg1[5] = inputArg1[20];
  outputArg1[57] = inputArg1[42];
  outputArg1[59] = inputArg1[53];
  outputArg1[61] = inputArg1[60];
  outputArg1[2] = inputArg1[3];
  outputArg1[4] = inputArg1[10];
  outputArg1[6] = inputArg1[21];
  outputArg1[58] = inputArg1[43];
  outputArg1[60] = inputArg1[54];
  outputArg1[62] = inputArg1[61];
  outputArg1[63] = inputArg1[63];
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
    while(1) {
	for(int i = 0; i < 64; ++i) {
		inputArg1[i] =fgetc(input_1);
	}

	mdl_inv_zigzag_195_hw( inputArg1, outputArg1 );

	for(int i = 0; i < 64; ++i) {
		fputc(outputArg1[i], output_1);
	}
    }
}
