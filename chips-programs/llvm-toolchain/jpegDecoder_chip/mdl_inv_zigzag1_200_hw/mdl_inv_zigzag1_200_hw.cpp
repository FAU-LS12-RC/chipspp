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
#ifdef DOUBLE_IDCT
int outputArg2 output("outputArg2");
#endif

void main() {

	int16_T input_1[64];
	int16_T output_1[64];

    int idct_id = 0;

	while(1) {
#ifdef ASYNCIO
      asyncio_read(inputArg1, input_1, sizeof(input_1) / 4);
      asyncio_wait_read();
#else
		for(int i = 0; i < 64; ++i) {
			input_1[i] =fgetc(inputArg1);
		}
#endif

		mdl_inv_zigzag1_200_hw( input_1, output_1 );

#ifdef ASYNCIO
      if(idct_id == 0) {
        asyncio_write(outputArg1, output_1, sizeof(output_1) / 4);
        asyncio_wait_write();
      }
#ifdef DOUBLE_IDCT
      else {
        asyncio_write(outputArg2, output_1, sizeof(output_1) / 4);
        asyncio_wait_write();
      }
      idct_id = !idct_id;
#endif
#else
		for(int i = 0; i < 64; ++i) {
			fputc(output_1[i], outputArg1);
		}
#endif
	}
}
