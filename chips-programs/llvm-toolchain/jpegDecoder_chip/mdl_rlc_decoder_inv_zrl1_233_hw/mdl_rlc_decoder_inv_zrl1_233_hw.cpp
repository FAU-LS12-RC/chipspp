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

void main() {

	uint32_T input_1[15];
	int16_T output_1[64];

	while(1) {
#ifdef ASYNCIO
      asyncio_read(inputArg1, input_1, sizeof(input_1) / 4);
      asyncio_wait_read();
#else
		for(int i = 0; i < 15; ++i) {
			input_1[i] =fgetc(inputArg1);
		}
#endif

		mdl_rlc_decoder_inv_zrl1_233_hw( input_1, output_1 );

#ifdef ASYNCIO
      asyncio_write(outputArg1, output_1, sizeof(output_1) / 4);
      asyncio_wait_write();
#else
		for(int i = 0; i < 64; ++i) {
			fputc(output_1[i], outputArg1);
		}
#endif
	}
}
