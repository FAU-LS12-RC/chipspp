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
 * File: mdl_rlc_decoder_inv_zrl1_236.c
 *
 * Code generated for Simulink model 'mdl_rlc_decoder_inv_zrl1_236'.
 *
 * Model version                  : 1.1
 * Simulink Coder version         : 8.12 (R2017a) 16-Feb-2017
 * C/C++ source code generated on : Mon Oct  1 17:42:34 2018
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Custom Processor->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */
#include "mdl_rlc_decoder_inv_zrl1_236_hw.hpp"
/* Block states (auto storage) */
/* Model step function */
void mdl_rlc_decoder_inv_zrl1_236_hw(const uint32_T inputArg1[15], int16_T
  outputArg1[64])
{

DW_mdl_rlc_decoder_inv_zrl1_236_T mdl_rlc_decoder_inv_zrl1_236_DW;
for(int i = 0; i < 64; ++i)
  mdl_rlc_decoder_inv_zrl1_236_DW.prev_out[i] = 0;


static bool init=true;
if(init){
{
  /* prev_out = single(zeros(64,1)); */
  /* '<S2>:1:9' prev_out = int32(zeros(64,1)); */
}
  init=false;
}
  int8_T cbits;
  uint8_T inpos;
  uint32_T Temp;
  uint16_T run;
  uint16_T val;
  uint16_T cnt;
  int32_T lval;
  int32_T temp;
  uint8_T flag;
  uint16_T j;
  uint16_T varargin_1;
  uint32_T tmp;
  boolean_T exitg1;
  /* MATLAB Function: '<S1>/Bit Unpack' incorporates:
   *  Inport: '<Root>/In1'
   */
  /*  Unpack the bits packed by the bit pack block. If there are not enough */
  /*  values in the packed stream the rest of the output is filled with zeros. */
  /* MATLAB Function 'rlc_decoder_inv_zrl1/Bit Unpack': '<S2>:1' */
  /* '<S2>:1:7' if isempty(prev_out) */
  /* '<S2>:1:11' out = prev_out; */
  /* '<S2>:1:12' cbits= int8(0); */
  cbits = 0;
  /* '<S2>:1:13' inpos=uint8(2); */
  inpos = 2U;
  /* '<S2>:1:14' Temp = uint32(0); */
  Temp = 0U;
  /* '<S2>:1:15' run = uint16(0); */
  run = 0U;
  /* '<S2>:1:16' val = uint16(0); */
  val = 0U;
  /* numcoefs = 64; */
  /* '<S2>:1:20' if bitget(uint32(in(1)),32) */
  if ((inputArg1[0] & 2147483648U) != 0U) {
    /* negative */
    /* '<S2>:1:21' out(1)=-int32(intmax('uint32')- uint32(in(1))) - int32(1); */
    tmp = MAX_uint32_T - inputArg1[0];
    if (tmp > 2147483647U) {
      tmp = 2147483647U;
    }
    mdl_rlc_decoder_inv_zrl1_236_DW.prev_out[0] = -(int32_T)tmp - 1;
  } else {
    /* '<S2>:1:22' else */
    /* '<S2>:1:23' out(1)=int32(in(1)); */
    tmp = inputArg1[0];
    if (inputArg1[0] > 2147483647U) {
      tmp = 2147483647U;
    }
    mdl_rlc_decoder_inv_zrl1_236_DW.prev_out[0] = (int32_T)tmp;
  }
  /*  ac coefficients */
  /*  4 bit - run length followed by 8 bit coef value */
  /* '<S2>:1:28' cnt=uint16(2); */
  cnt = 2U;
  /* '<S2>:1:29' while (cnt<=64) */
  exitg1 = false;
  while ((!exitg1) && (cnt <= 64)) {
    /* numcoefs */
    /* [flag, cbits, inpos, Temp, run] = getbits(cbits, inpos, Temp, in, run, int8(4)); */
    /* '<S2>:1:31' bits=int8(4); */
    /* '<S2>:1:32' lval = uint32(0); */
    /* '<S2>:1:33' temp = uint32(0); */
    /* '<S2>:1:34' inwidth = length(in); */
    /* '<S2>:1:35' if (cbits < bits) */
    if (cbits < 4) {
      /* '<S2>:1:36' if (inpos < inwidth) */
      if (inpos < 15) {
        /* '<S2>:1:37' lval(:) =  bitshift(Temp, -(32-cbits)); */
        if (32 - cbits <= 31) {
          lval = (int32_T)(Temp >> (32 - cbits));
        } else {
          lval = 0;
        }
        /* '<S2>:1:38' bits(:) = int8(bits) - int8(cbits); */
        /* '<S2>:1:39' Temp = uint32(in(inpos)); */
        Temp = inputArg1[inpos - 1];
        /* '<S2>:1:40' inpos = inpos +1; */
        inpos++;
        /* '<S2>:1:41' lval = bitshift(lval, bits); */
        lval = (int32_T)((uint32_T)lval << (4 - cbits));
        /* '<S2>:1:42' cbits(:) = int8(32); */
        /* '<S2>:1:44' temp(:) = bitshift(Temp, -(32-bits)); */
        temp = (int32_T)(Temp >> (cbits + 28));
        /* '<S2>:1:45' Temp(:) = bitshift(Temp, bits); */
        Temp <<= (int8_T)(4 - cbits);
        /* '<S2>:1:46' run(:) = bitor(lval, temp); */
        run = (uint16_T)((uint32_T)lval | temp);
        /* '<S2>:1:47' cbits(:) = int8(cbits) - int8(bits); */
        cbits = (int8_T)(cbits + 28);
        /* '<S2>:1:48' flag=uint8(1); */
        flag = 1U;
      } else {
        /* '<S2>:1:49' else */
        /* '<S2>:1:50' flag=uint8(0); */
        flag = 0U;
      }
    } else {
      /* '<S2>:1:52' else */
      /* '<S2>:1:53' temp(:) = bitshift(Temp, -(32-bits)); */
      temp = (int32_T)(Temp >> 28);
      /* '<S2>:1:54' Temp(:) = bitshift(Temp, bits); */
      Temp <<= 4;
      /* '<S2>:1:55' run(:) = bitor(lval, temp); */
      run = (uint16_T)temp;
      /* '<S2>:1:56' cbits(:) = int8(cbits) - int8(bits); */
      cbits = (int8_T)(cbits - 4);
      /* '<S2>:1:57' flag=uint8(1); */
      flag = 1U;
    }
    /* '<S2>:1:60' if (~flag) */
    if ((!(flag != 0)) || (run == 15)) {
      exitg1 = true;
    } else {
      /* '<S2>:1:64' if (run == uint16(15)) */
      /*  end of stream */
      /* [flag, cbits, inpos, Temp, val] = getbits(cbits, inpos, Temp, in, val, int8(8)); */
      /* '<S2>:1:67' bits=int8(8); */
      /* '<S2>:1:68' lval = uint32(0); */
      /* '<S2>:1:69' temp = uint32(0); */
      /* '<S2>:1:70' inwidth = length(in); */
      /* '<S2>:1:71' if (cbits < bits) */
      if (cbits < 8) {
        /* '<S2>:1:72' if (inpos < inwidth) */
        if (inpos < 15) {
          /* '<S2>:1:73' lval(:) =  bitshift(Temp, -(32-cbits)); */
          if (32 - cbits <= 31) {
            lval = (int32_T)(Temp >> (32 - cbits));
          } else {
            lval = 0;
          }
          /* '<S2>:1:74' bits(:) = int8(bits) - int8(cbits); */
          /* '<S2>:1:75' Temp = uint32(in(inpos)); */
          Temp = inputArg1[inpos - 1];
          /* '<S2>:1:76' inpos = inpos +1; */
          inpos++;
          /* '<S2>:1:77' lval = bitshift(lval, bits); */
          lval = (int32_T)((uint32_T)lval << (8 - cbits));
          /* '<S2>:1:78' cbits(:) = int8(32); */
          /* '<S2>:1:80' temp(:) = bitshift(Temp, -(32-bits)); */
          temp = (int32_T)(Temp >> (cbits + 24));
          /* '<S2>:1:81' Temp(:) = bitshift(Temp, bits); */
          Temp <<= (int8_T)(8 - cbits);
          /* '<S2>:1:82' val(:) = bitor(lval, temp); */
          val = (uint16_T)((uint32_T)lval | temp);
          /* '<S2>:1:83' cbits(:) = int8(cbits) - int8(bits); */
          cbits = (int8_T)(cbits + 24);
          /* '<S2>:1:84' flag=uint8(1); */
        } else {
          /* '<S2>:1:85' else */
          /* '<S2>:1:86' flag=uint8(0); */
          flag = 0U;
        }
      } else {
        /* '<S2>:1:88' else */
        /* '<S2>:1:89' temp(:) = bitshift(Temp, -(32-bits)); */
        temp = (int32_T)(Temp >> 24);
        /* '<S2>:1:90' Temp(:) = bitshift(Temp, bits); */
        Temp <<= 8;
        /* '<S2>:1:91' val(:) = bitor(lval, temp); */
        val = (uint16_T)temp;
        /* '<S2>:1:92' cbits(:) = int8(cbits) - int8(bits); */
        cbits = (int8_T)(cbits - 8);
        /* '<S2>:1:93' flag=uint8(1); */
      }
      /* '<S2>:1:96' if (~flag) */
      if (!(flag != 0)) {
        exitg1 = true;
      } else {
        /* '<S2>:1:100' for j= cnt: min(run+cnt, 64) */
        varargin_1 = (uint16_T)(run + cnt);
        if (!(varargin_1 < 64)) {
          varargin_1 = 64U;
        }
        for (j = cnt; j <= varargin_1; j++) {
          /* numcoefs */
          /* out(j) = single(single(val)-128.0); */
          /* '<S2>:1:102' out(j) = int32(int32(val)-int32(128)); */
          mdl_rlc_decoder_inv_zrl1_236_DW.prev_out[j - 1] = val - 128;
        }
        /* '<S2>:1:104' cnt = cnt + uint16(run)+1; */
        cnt = (uint16_T)((cnt + run) + 1);
      }
    }
  }
  /*  when we havent got all coefs in the stream */
  /* '<S2>:1:108' for j=cnt:64 */
  for (run = cnt; run < 65; run++) {
    /* numcoefs */
    /* '<S2>:1:109' out(cnt) = 0; */
    mdl_rlc_decoder_inv_zrl1_236_DW.prev_out[cnt - 1] = 0;
  }
  /* End of MATLAB Function: '<S1>/Bit Unpack' */
  /* Outport: '<Root>/Out1' incorporates:
   *  DataTypeConversion: '<S1>/Data Type Conversion'
   */
  /* '<S2>:1:111' prev_out(:)=out; */
  /* ================================================= */
  /*  function [flag, cbitsO, inposO, TempO, valO] = getbits(cbits, inpos, Temp, in, val, bits) */
  /*  */
  /*      lval = uint32(0); */
  /*      temp = uint32(0); */
  /*      cbitsO = int8(cbits); */
  /*      inposO = inpos; */
  /*      TempO = uint32(Temp); */
  /*      valO = uint16(val); */
  /*      inwidth = length(in); */
  /*      if (cbitsO < bits) */
  /*          if (inposO > inwidth) */
  /*              flag=uint8(0); */
  /*              return; */
  /*          end */
  /*  */
  /*          lval(:) =  bitshift(TempO, -(32-cbitsO)); */
  /*          bits(:) = int8(bits) - int8(cbitsO); */
  /*          TempO = in(inposO); */
  /*          inposO = inposO +1; */
  /*          lval = bitshift(lval, bits); */
  /*          cbitsO(:) = int8(32); */
  /*      end */
  /*  */
  /*      temp(:) = bitshift(TempO, -(32-bits)); */
  /*      TempO(:) = bitshift(TempO, bits); */
  /*      valO(:) = bitor(lval, temp); */
  /*      cbitsO(:) = int8(cbitsO) - int8(bits); */
  /*  */
  /*      flag = uint8(1); */
  /*  end */
  for (temp = 0; temp < 64; temp++) {
    outputArg1[temp] = (int16_T)mdl_rlc_decoder_inv_zrl1_236_DW.prev_out[temp];
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
int input_1 input("input_1");
int output_1 output("output_1");

void main() {

	uint32_T inputArg1[15];
	int16_T outputArg1[64];
    while(1) {
	for(int i = 0; i < 15; ++i) {
		inputArg1[i] =fgetc(input_1);
	}

	mdl_rlc_decoder_inv_zrl1_236_hw( inputArg1, outputArg1 );

	for(int i = 0; i < 64; ++i) {
		fputc(outputArg1[i], output_1);
	}
    }
}
