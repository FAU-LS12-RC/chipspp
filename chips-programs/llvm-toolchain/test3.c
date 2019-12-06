#include <stdio.h>

typedef float real32_T;

const int input("input_1") input_1;
const int output("output_1") output_1;

void fft_custom(float const in[256]) {
  for(int i = 0; i < 256; i++) {
    report((int)in[i]);
  }
}

void main() {
  real32_T in_buffer[256];
  while(1) {
    for(int i = 0; i < 256; ++i) {
      in_buffer[i] = fget_float(input_1);
      //report((int)in_buffer[i]);
    }
    fft_custom(in_buffer);

    for(int i = 0; i < 256; ++i) {
      fput_float(0.0f, output_1);
      fput_float(0.0f, output_1);
    }
  }
}
