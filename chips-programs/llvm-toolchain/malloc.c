//#define HEAP_SIZE 800
#include <stdlib.h>

int const input_1 input("input_1");
int const output_1 output("output_1");

struct foo {
  int a;
  int b;
  int c;
  struct foo* next;
};

void main() {
  int i;
  struct foo* first = 0;
  struct foo* last = 0;
  struct foo* current = 0;
//  _malloc_init();
  while(fgetc(input_1) != 0) {
    current = malloc(sizeof(struct foo));
    if(!first)
      first = current;
    last->next = current;
    current->a = 0;
    current->b = fgetc(input_1);
    //report(current->b);
    current->c = 0;
    current->next = 0;
    last = current;
  }

  current = first;
  for(current = first; current; current = current->next) {
    //free(last);
    fputc(current->b, output_1);
  }
  //free(last);
}
