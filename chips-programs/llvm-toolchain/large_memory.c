//#define SIZE 16000

unsigned char array_1[SIZE];
unsigned char array_2[SIZE];

int input_1 input("input_1");
int output_1 output("output_1");

void main() {
  int i_1 = fgetc(input_1);
  int i_2 = fgetc(input_1);

  report(1000);

  for(int i = 0; i < SIZE; ++i)
    array_1[i] = i_1;
  report(2000);
  for(int i = 0; i < SIZE; ++i)
    array_2[i] = i_2;

  fputc(array_1[SIZE / 2], output_1);
  fputc(array_2[SIZE / 2], output_1);
}
