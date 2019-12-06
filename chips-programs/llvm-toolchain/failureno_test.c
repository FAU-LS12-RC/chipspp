#include <failureno.h>

void main2(int, int);

int input_1 input("input_1");
int output_1 output("output_1");


void main(void) {
  int n = fgetc(input_1);
  int ret = foo(n);
  if(ret == -1 && failureno == EINVAL)
    fputc(0, output_1);
  else
    fputc(1, output_1);

  main2(n, output_1);
}
