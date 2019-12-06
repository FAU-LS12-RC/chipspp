int const input_1 input("input_1");
int const output_1 output("output_1");

struct foo {
  int a;
  int b;
  int c;
  int d;
  int e;
};

void copy(struct foo* dst, struct foo* src) {
  *dst = *src;
}

void main(void) {
  struct foo src;
  struct foo dst;
  int i = fgetc(input_1);
  src.a = fgetc(input_1);
  src.b = fgetc(input_1);
  src.c = fgetc(input_1);
  src.d = fgetc(input_1);
  src.e = fgetc(input_1);

  copy(&dst, &src + i);

  fputc(dst.a, output_1);
  fputc(dst.b, output_1);
  fputc(dst.c, output_1);
  fputc(dst.d, output_1);
  fputc(dst.e, output_1);
  fputc(0, output_1);
}
