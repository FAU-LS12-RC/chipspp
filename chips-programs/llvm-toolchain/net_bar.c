int input_1 input("input_1");
int output_1 output("output_1");

void main(void) {
  fputc(fgetc(input_1) + 2, output_1);
}
