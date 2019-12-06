#define NO_OF_CHANNELS 3

void before_idct(unsigned char buf[2][NO_OF_CHANNELS][128], int* idct_outs) {
  int i;

  asyncio_read(inputArg1, (uint32_t*)buf[0], 45);
  asyncio_wait_read();

  mdl_img_splitter_66_hw((uint32_t*)buf[0], (uint32_t*)buf[1][0],
      (uint32_t*)buf[1][1], (uint32_t*)buf[1][2]);

  for(i = 0; i < NO_OF_CHANNELS; ++i)
    mdl_rlc_decoder_inv_zrl1_233_hw((uint32_t*)buf[1][i],
        (int16_t*)buf[0][i]);
  for(i = 0; i < NO_OF_CHANNELS; ++i)
    mdl_inv_zigzag1_200_hw((int16_t*)buf[0][i], (int16_t*)buf[1][i]);

  for(i = 0; i < NO_OF_CHANNELS; ++i) {
    asyncio_write(idct_outs[i], buf[1][i], 32);
    asyncio_wait_write();
  }
}

void after_idct(unsigned char buf[2][NO_OF_CHANNELS][128], int* idct_ins) {
  int i;

  for(i = 0; i < NO_OF_CHANNELS; ++i) {
    asyncio_read(idct_ins[i], buf[0][i], 16);
    asyncio_wait_read();
  }

  mdl_colorspaceconv_4_hw((uint8_t*)buf[0][0], (uint8_t*)buf[0][1],
      (uint8_t*)buf[0][2], (uint8_t*)buf[1]);

  asyncio_write(outputArg1, buf[1], 192 / 4);
  asyncio_wait_write();
}

