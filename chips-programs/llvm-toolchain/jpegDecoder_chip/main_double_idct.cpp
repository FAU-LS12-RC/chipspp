#include <mdl_colorspaceconv_4_hw/impl.cpp>
#include <mdl_img_splitter_66_hw/impl.cpp>
#include <mdl_inv_quant_idct1_126_hw/impl.cpp>
#include <mdl_inv_zigzag1_200_hw/impl.cpp>
#include <mdl_rlc_decoder_inv_zrl1_233_hw/impl.cpp>

#include <asyncio.h>
#include <stdint.h>

int inputArg1 input("inputArg1");
int outputArg1 output("outputArg1");

int output_idct_1_1 output("output_idct_1_1");
int output_idct_1_2 output("output_idct_1_2");
int output_idct_2_1 output("output_idct_2_1");
int output_idct_2_2 output("output_idct_2_2");
int output_idct_3_1 output("output_idct_3_1");
int output_idct_3_2 output("output_idct_3_2");

int input_idct_1_1 input("input_idct_1_1");
int input_idct_1_2 input("input_idct_1_2");
int input_idct_2_1 input("input_idct_2_1");
int input_idct_2_2 input("input_idct_2_2");
int input_idct_3_1 input("input_idct_3_1");
int input_idct_3_2 input("input_idct_3_2");

//setup:
//1: before
//start 1
//
//steady state:
//i: before
//start i
//wait for i - 1
//i - 1: after 

#include <main.hpp>

void main(void) {
  unsigned char buf[2][2][NO_OF_CHANNELS][128];

  int idct_ins[][NO_OF_CHANNELS] = { 
    { input_idct_1_1, input_idct_2_1, input_idct_3_1 },
    { input_idct_1_2, input_idct_2_2, input_idct_3_2 } };
  int idct_outs[][NO_OF_CHANNELS] = {
    { output_idct_1_1, output_idct_2_1, output_idct_3_1 },
    { output_idct_1_2, output_idct_2_2, output_idct_3_2 } };

  int buffer_id = 0;

  before_idct(buf[0], idct_outs[0]);
  buffer_id = 1;
  while(1) {
    before_idct(buf[buffer_id], idct_outs[buffer_id]);
    after_idct(buf[!buffer_id], idct_ins[!buffer_id]);
    buffer_id = !buffer_id;
  }

  //TODO double buffer
}
