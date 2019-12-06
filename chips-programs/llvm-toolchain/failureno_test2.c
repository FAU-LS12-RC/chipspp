#include <failureno.h>

void main2(int n, int output) {
  int ret = foo(n);
  if(ret == -1 && failureno == EINVAL)
    fputc(0, output);
  else
    fputc(1, output);
}
