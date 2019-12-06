#include <stdio.h>
const int input_1 = input("input_1");
const int output_1 = output("output_1");

const int ArraySize = 10;
int array[ArraySize];

void swap(int i1, int i2) {
  int temp = array[i1];
  array[i1] = array[i2];
  array[i2] = temp;
//  report(100 + i1);
//  report(100 + i2);
}

void quicksort(int left, int right) {
  int pivot;
  int len = right - left;
  int pivot_i = right - 1;
  int i = left;
  int smaller_i = left;
  if(len <= 1)
    return;
//  report(10000);
//  report(left);
//  report(right);
//  report(array[0]);
//  report(array[1]);
//  report(array[2]);
//  report(array[3]);
//  report(array[4]);
//  report(array[5]);
//  report(array[6]);
//  report(array[7]);
//  report(array[8]);
//  report(array[9]);

  //TODO test loop optimizations
  pivot = array[pivot_i];
  for(i = left; i < right; ++i) {
    if(array[i] < pivot) {
      swap(smaller_i, i);
      smaller_i += 1;
    }
  }

  swap(smaller_i, pivot_i);
  //report(200);
  quicksort(left, smaller_i);
  quicksort(smaller_i + 1, right);
  return;
}

void main() {
  int i;
  while(1) {
    for(i = 0; i < ArraySize; ++i)
      array[i] = fgetc(input_1);

    quicksort(0, ArraySize);

    for(i = 0; i < ArraySize; ++i)
      fputc(array[i], output_1);
  }
}

//void main() {
//    while(1) {
//        fput_double(fget_double(input_1) + 1.0, output_1);
//    }
//}
//
//input_1 = input("input_1");
//input_2 = input("input_2");
//output_1 = output("output_1");
//void main() {
//  while(1) {
//    fputc(fgetc(input_1) + fgetc(input_2), output_1);
//  }
//}
