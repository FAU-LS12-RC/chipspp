#define ENABLE_DYNAMIC_MEMORY
#define MALLOC_SIZE 100
#include <stdlib.h>

int const input_1 input("input_1");
int const output_1 output("output_1");

unsigned char* fgets() {
  int len = fgetc(input_1);
  unsigned char* string = malloc(len);
  for(int i = 0; i < len; ++i)
    string[i] = fgetc(input_1);
  return string;
}

unsigned char const_string[] = "abc";

void main() {
  _malloc_init();

  unsigned char* strings[3];

  for(int i = 0; i < 3; ++i)
    strings[i] = fgets();

  for(int i = 0; i < sizeof(const_string); ++i)
    fputc(const_string[i], output_1);

  for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j)
      fputc(strings[j][i], output_1);
  fputc(0, output_1);
}
