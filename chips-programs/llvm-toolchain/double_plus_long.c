#include <stdio.h>
#include <stdint.h>

int input_1 input("input_1");
int output_1 output("output_1");

void main(void) {
  while(1) {
    int64_t n = fget_double(input_1);
    fput_long(n + (int64_t)1e12, output_1);
  }
}
