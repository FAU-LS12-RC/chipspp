#include <stdio.h>

int input_1 input("input_1");
int input_2 input("input_2");
int input_3 input("input_3");
int output_1 output("output_1");
int output_2 output("output_2");

void main(void) {
  while(1) {
    float float_value = fget_float(input_2);
    fput_long(fgetc(input_1) + float_value, output_1);
    fput_float(float_value + fget_long(input_3), output_2);
  }
}
