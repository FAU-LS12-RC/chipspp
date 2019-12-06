#include <mdl_colorspaceconv_4_hw/impl.cpp>
#include <mdl_img_splitter_66_hw/impl.cpp>
#include <mdl_inv_quant_idct1_126_hw/impl.cpp>
#include <mdl_inv_zigzag1_200_hw/impl.cpp>
#include <mdl_rlc_decoder_inv_zrl1_233_hw/impl.cpp>

#include <asyncio.h>
#include <stdint.h>

int inputArg1 input("inputArg1");
int outputArg1 output("outputArg1");

int output_idct_1 output("output_idct_1");
int output_idct_2 output("output_idct_2");
int output_idct_3 output("output_idct_3");

int input_idct_1 input("input_idct_1");
int input_idct_2 input("input_idct_2");
int input_idct_3 input("input_idct_3");

#include <main.hpp>

void main(void) {
  unsigned char buf[2][NO_OF_CHANNELS][128];

  int idct_ins[NO_OF_CHANNELS] = {
    input_idct_1, input_idct_2, input_idct_3
  };
  int idct_outs[NO_OF_CHANNELS] = {
    output_idct_1, output_idct_2, output_idct_3
  };
  
  //TODO double buffer
  while(1) {
    before_idct(buf, idct_outs);
    after_idct(buf, idct_ins);
  }
}
