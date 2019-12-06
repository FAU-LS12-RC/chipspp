#include <impl.cpp>

/* Model initialize function */
/* Model terminate function */
/*
 * File trailer for generated code.
 *
 * [EOF]
 */
#ifdef ASYNCIO
#include <asyncio.h>
#endif

int inputArg1 input("inputArg1");
int outputArg1 output("outputArg1");
int outputArg2 output("outputArg2");
int outputArg3 output("outputArg3");
void main() {

	uint32_T input_1[45];
	uint32_T output_1[15];
	uint32_T output_2[15];
	uint32_T output_3[15];

	while(1) {
#ifdef ASYNCIO
      asyncio_read(inputArg1, input_1, sizeof(input_1) / 4);
      asyncio_wait_read();
#else
		for(int i = 0; i < 45; ++i) {
			input_1[i] =fgetc(inputArg1);
		}
#endif

		mdl_img_splitter_66_hw( input_1, output_1, output_2, output_3 );

#ifdef ASYNCIO
      asyncio_write(outputArg1, output_1, sizeof(output_1) / 4);
      asyncio_wait_write();
      asyncio_write(outputArg2, output_2, sizeof(output_2) / 4);
      asyncio_wait_write();
      asyncio_write(outputArg3, output_3, sizeof(output_3) / 4);
      asyncio_wait_write();
#else
		for(int i = 0; i < 15; ++i) {
			fputc(output_1[i], outputArg1);
		}
		for(int i = 0; i < 15; ++i) {
			fputc(output_2[i], outputArg2);
		}
		for(int i = 0; i < 15; ++i) {
			fputc(output_3[i], outputArg3);
		}
#endif
	}
}
