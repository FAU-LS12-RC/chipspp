int input_1 input("input_1");
int output_1 output("output_1");

int bar(int n) {
  char local[20];

  int ret = 2 * n;
  return ret;
}

int foo(int n) {
  char local[10];

  return bar(n) + 1;
}

int compute(int n) {
  char local[30];

  int a = foo(n);
  int b = bar(n);
  return a + b; //4n + 1
}

void main(void) {
  char local[50];

  fputc(compute(fgetc(input_1)), output_1);
}
