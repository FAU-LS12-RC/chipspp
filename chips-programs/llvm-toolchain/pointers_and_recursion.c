int const input_1 input("input_1");
int const output_1 output("output_1");

#define bool unsigned char

void process(int len, int* array, int i, bool do_anything) {
  if(!do_anything)
    return;
  if(i >= len)
    return;
  array[i] += 1;
  process(len, array, i + 1, do_anything);
}

void main(void) {
  int array[20];
  bool do_anything = fgetc(input_1);
  int length = fgetc(input_1);
  for(int i = 0; i < length; ++i) {
    report(i);
    array[i] = fgetc(input_1);
  }
  process(length, array, 0, do_anything);
  for(int i = 0; i < length; ++i)
    fputc(array[i], output_1);
}
