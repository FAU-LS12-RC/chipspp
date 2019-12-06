int input_1 = input("input_1");
int input_2 = input("input_2");
int output_1 = output("output_1");
void main() {
  while(1) {
    fputc((int)fgetc(input_1) + (int)fgetc(input_2), output_1);
  }
}
