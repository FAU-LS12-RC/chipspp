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

#ifdef DOUBLE_BUF_IDCT
#include <doublebuf.h>
DOUBLEBUF_INPUT(int16_T, inputArg1, 64)
DOUBLEBUF_OUTPUT(uint8_T, outputArg1, 64)
void compute(int buffer_id) {
  int16_T* input_1 = (int16_T*)DOUBLEBUF_BUFFER(buffer_id, inputArg1);
  uint8_T* output_1 = (uint8_T*)DOUBLEBUF_BUFFER(buffer_id, outputArg1);

  mdl_inv_quant_idct1_126_hw(input_1, output_1);
}
#else
int inputArg1 input("inputArg1");
int outputArg1 output("outputArg1");
#endif

void main() {

#ifdef DOUBLE_BUF_IDCT
  DOUBLEBUF_PORT_INPUTS {
    DOUBLEBUF_PORT_NAME(inputArg1)
  };
  DOUBLEBUF_PORT_OUTPUTS {
    DOUBLEBUF_PORT_NAME(outputArg1)
  };
//  report((int)&inputArg1_data_1);
//  report((int)&inputArg1_data_2);
//  report((int)inputArg1_port_def.back_buf);
//  report((int)doublebuf_inputs[0]->front_buf);
//  report((int)doublebuf_inputs[0]->back_buf);

  DOUBLEBUF_COMPUTE_LOOP(compute, -1);

#else

	int16_T input_1[64];
	uint8_T output_1[64];

	while(1) {
#ifndef ASYNCIO
		for(int i = 0; i < 64; ++i) {
			input_1[i] =fgetc(inputArg1);
		}
#else
        asyncio_read(inputArg1, input_1, sizeof(input_1) / 4);
        asyncio_wait_read();
#endif

		mdl_inv_quant_idct1_126_hw( input_1, output_1 );

#ifndef ASYNCIO
		for(int i = 0; i < 64; ++i) {
			fputc(output_1[i], outputArg1);
		}
#else
        asyncio_write(outputArg1, output_1, sizeof(output_1) / 4);
        asyncio_wait_write();
#endif
	}
#endif //Double buf
}
