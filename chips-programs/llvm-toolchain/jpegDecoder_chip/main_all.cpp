#include <mdl_colorspaceconv_4_hw/impl.cpp>
#include <mdl_img_splitter_66_hw/impl.cpp>
#include <mdl_inv_quant_idct1_126_hw/impl.cpp>
#include <mdl_inv_zigzag1_200_hw/impl.cpp>
#include <mdl_rlc_decoder_inv_zrl1_233_hw/impl.cpp>

#include <asyncio.h>
#include <stdint.h>

#define SIMPLE 0
#define ASYNC 1
#ifndef IO
#define IO ASYNC
#endif

void compute_channel(uint32_t* splitter_out, uint8_t* out) {
  int16_t zrl_out[64];
  mdl_rlc_decoder_inv_zrl1_233_hw(splitter_out, zrl_out);
  int16_t zigzag_out[64];
  mdl_inv_zigzag1_200_hw(zrl_out, zigzag_out);
  
  //TODO channel of ... or 
}

int inputArg1 input("inputArg1");
int outputArg1 output("outputArg1");

void main(void) {
  uint32_t input_data[45];

  uint32_t splitter_out_1[15];
  uint32_t splitter_out_2[15];
  uint32_t splitter_out_3[15];

  int16_t zrl_out_1[64];
  int16_t zrl_out_2[64];
  int16_t zrl_out_3[64];
  int16_t zigzag_out_1[64];
  int16_t zigzag_out_2[64];
  int16_t zigzag_out_3[64];
  uint8_t idct_out_1[64];
  uint8_t idct_out_2[64];
  uint8_t idct_out_3[64];
  uint8_t out[192];
  
  //TODO double buffer
  while(1) {
#if IO == SIMPLE
    for(int i = 0; i < sizeof(input_data) / 4; ++i)
      input_data[i] = fgetc(inputArg1);
#else
    asyncio_read(inputArg1, input_data, sizeof(input_data) / 4);
    asyncio_wait_read();
#endif

    mdl_img_splitter_66_hw(input_data, splitter_out_1, splitter_out_2,
        splitter_out_3);

  //TODO reuse buffers

    mdl_rlc_decoder_inv_zrl1_233_hw(splitter_out_1, zrl_out_1);
    mdl_rlc_decoder_inv_zrl1_233_hw(splitter_out_2, zrl_out_2);
    mdl_rlc_decoder_inv_zrl1_233_hw(splitter_out_3, zrl_out_3);

    mdl_inv_zigzag1_200_hw(zrl_out_1, zigzag_out_1);
    mdl_inv_zigzag1_200_hw(zrl_out_2, zigzag_out_2);
    mdl_inv_zigzag1_200_hw(zrl_out_3, zigzag_out_3);

    mdl_inv_quant_idct1_126_hw(zigzag_out_1, idct_out_1);
    mdl_inv_quant_idct1_126_hw(zigzag_out_2, idct_out_2);
    mdl_inv_quant_idct1_126_hw(zigzag_out_3, idct_out_3);

    mdl_colorspaceconv_4_hw(idct_out_1, idct_out_2, idct_out_3, out);

#if IO == SIMPLE
    for(int i = 0; i < sizeof(out) / 4; ++i)
      fputc(((uint32_t*)out)[i], outputArg1);
#else
    asyncio_write(outputArg1, out, sizeof(out) / 4);
    asyncio_wait_write();
#endif
  }
}
