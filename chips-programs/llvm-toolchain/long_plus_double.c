#include <stdio.h>

int input_1 input("input_1");
int output_1 output("output_1");

void main(void) {
  while(1) {
    double x = fget_long(input_1);
    fput_double(x + 1.5, output_1);
  }
}
