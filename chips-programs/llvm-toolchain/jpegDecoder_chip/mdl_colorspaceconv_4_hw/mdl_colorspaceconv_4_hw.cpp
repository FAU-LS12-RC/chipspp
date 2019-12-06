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
int inputArg2 input("inputArg2");
int inputArg3 input("inputArg3");
#ifdef DOUBLE_IDCT
int inputArg4 input("inputArg4");
int inputArg5 input("inputArg5");
int inputArg6 input("inputArg6");
#endif
int outputArg1 output("outputArg1");
void main() {

	uint8_T input_1[64];
	uint8_T input_2[64];
	uint8_T input_3[64];
	uint8_T output_1[192];

    int idct_id = 0;

	while(1) {
#ifdef ASYNCIO
      if(idct_id == 0) {
        asyncio_read(inputArg1, input_1, sizeof(input_1) / 4);
        asyncio_wait_read();
        asyncio_read(inputArg2, input_2, sizeof(input_2) / 4);
        asyncio_wait_read();
        asyncio_read(inputArg3, input_3, sizeof(input_3) / 4);
        asyncio_wait_read();
      } 
#ifdef DOUBLE_IDCT
      else {
        asyncio_read(inputArg4, input_1, sizeof(input_1) / 4);
        asyncio_wait_read();
        asyncio_read(inputArg5, input_2, sizeof(input_2) / 4);
        asyncio_wait_read();
        asyncio_read(inputArg6, input_3, sizeof(input_3) / 4);
        asyncio_wait_read();
      }
      idct_id = !idct_id;
#endif
#else
		for(int i = 0; i < 64; ++i) {
			input_1[i] =fgetc(inputArg1);
		}
		for(int i = 0; i < 64; ++i) {
			input_2[i] =fgetc(inputArg2);
		}
		for(int i = 0; i < 64; ++i) {
			input_3[i] =fgetc(inputArg3);
		}
#endif

		mdl_colorspaceconv_4_hw( input_1, input_2, input_3, output_1 );

#ifdef ASYNCIO
      asyncio_write(outputArg1, output_1, sizeof(output_1) / 4);
      asyncio_wait_write();
#else
		for(int i = 0; i < 192; ++i) {
			fputc(output_1[i], outputArg1);
		}
#endif
	}
}
