int output_1 = output("output_1");

int func(int a) {
  return a + 10;
}

void main() {
  fputc(func(20), output_1);
}
