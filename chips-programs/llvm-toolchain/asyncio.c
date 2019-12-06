#include <asyncio.h>

#define SIZE 5000

int input_1 input("input_1");
int input_2 input("input_2");

int output_1 output("output_1");
int output_2 output("output_2");

int input_1_data[SIZE];
int input_2_data[SIZE];

void main(void) {
  asyncio_read(input_1, input_1_data, SIZE);
  asyncio_wait_read();

  asyncio_write(output_2, input_1_data, SIZE);

  asyncio_read(input_2, input_2_data, SIZE);
  asyncio_wait_read();
  fputc(input_2_data[SIZE-1], output_1);
  asyncio_wait_write();
}

//Programmed I/O
//void main(void) {
////  asyncio_read(input_1, input_1_data, SIZE);
////  asyncio_wait_read();
//  for(int i = 0; i < SIZE; ++i)
//    input_1_data[i] = fgetc(input_1);
//
////  asyncio_write(output_2, input_1_data, SIZE);
////  asyncio_wait_write();
//  for(int i = 0; i < SIZE; ++i)
//    fputc(input_1_data[i], output_2);
//
////  asyncio_read(input_2, input_2_data, SIZE);
////  asyncio_wait_read();
//  for(int i = 0; i < SIZE; ++i)
//    input_2_data[i] = fgetc(input_2);
//
//  fputc(input_2_data[SIZE-1], output_1);
//}



//void main(void) {
//  asyncio_read(input_1, input_1_data, SIZE);
//
//  wait_clocks(10);
//
//  for(int i = 0; i < SIZE; ++i)
//    fputc(input_1_data[i], output_2);
//  asyncio_wait_read();
//
////  asyncio_write(output_2, input_1_data, SIZE);
//
//  asyncio_read(input_2, input_2_data, SIZE);
//  asyncio_wait_read();
////  asyncio_wait_write();
//
//  fputc(input_2_data[SIZE-1], output_1);
//}
