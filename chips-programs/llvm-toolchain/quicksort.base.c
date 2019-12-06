#include <stdio.h>

//typedef unsigned int index;

const index ArraySize = 10;
static word array[ArraySize];

void swap(index i1, index i2) {
  word temp = array[i1];
  array[i1] = array[i2];
  array[i2] = temp;
}

void quicksort(index left, index right) {
  index len = right - left;
  index pivot_i = right - 1;
  index i = left;
  index smaller_i = left;
  if(len <= 1)
    return;

  for(i = left; i < right; ++i) {
    if(array[i] < array[pivot_i]) {
      swap(smaller_i, i);
      smaller_i += 1;
    }
  }

  swap(smaller_i, pivot_i);
  quicksort(left, smaller_i);
  quicksort(smaller_i + 1, right);
  return;
}

void main() {
  index i;
  while(1) {
    for(i = 0; i < ArraySize; ++i)
      array[i] = fgetc(input_1);

    quicksort(0, ArraySize);

    for(i = 0; i < ArraySize; ++i)
      fputc(array[i], output_1);
  }
}
