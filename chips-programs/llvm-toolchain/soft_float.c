#include <stdio.h>

int input_1 input("input_1");
int output_1 output("output_1");

void main(void) {
  while(1) {
    float x = fget_float(input_1);
    fput_float(x < 2.0f ? x + 1.0f : x, output_1);
  }
}
