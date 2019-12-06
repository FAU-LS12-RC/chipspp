int const input_1 = input("input_1");
int const output_1 = output("output_1");

#define bool unsigned char

//void process(int len, int* array, int i, bool do_anything) {
//  if(!do_anything)
//    return;
//  if(i >= len)
//    return;
//  array[i] += 1;
//  process(len, array, i + 1, do_anything);
//  return;
//}

void main() {
  int array[20];
  int i;
  for(i = 0; i < 10; ++i) {
    report(i);
    array[i] = fgetc(input_1);
  }
  //process(length, array, 0, do_anything);
  for(i = 0; i < 10; ++i)
    fputc(array[i], output_1);
}
