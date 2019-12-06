int input_1 input("input_1");
int output_1 output("output_1");

void main(void) {
  while(1) {
    int sum = fgetc(input_1) + fgetc(input_1);
    int shifted = sum << fgetc(input_1);
    int ret = shifted * 3;
    fputc(ret, output_1);
  }
}
