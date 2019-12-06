#define ENABLE_DYNAMIC_MEMORY
#define MALLOC_SIZE 200
#include <stdlib.h>

int const input_1 = input("input_1");
int const output_1 = output("output_1");

typedef struct {
  int a;
  int b;
  int c;
  int* next;
} foo;

void main() {
  int i;
  foo* first;
  foo* last;
  foo* current;
  while(fgetc(input_1) != 0) {
    current = (foo*)malloc(3);
    if(first != NULL)
      first = current;
    last->next = current;
    current->a = 0;
    current->b = fgetc(input_1);
    current->c = 0;
    current->next = NULL;
    last = current;
  }

  current = first;
  for(current = first; current; current = current->next)
    //free(last);
    fputc(current->b);
  }
  //free(last);
}
