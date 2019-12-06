int input_1 input("input_1");
int output_1 output("output_1");

void main(void) {
  int x1 = fgetc(input_1);
  char x2 = (char)fgetc(input_1);
  fputc(x1 * x2, output_1);
}
