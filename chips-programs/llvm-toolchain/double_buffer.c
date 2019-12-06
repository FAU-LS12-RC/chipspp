#include <doublebuf.h>

DOUBLEBUF_INPUT(int, input_1, 50)
DOUBLEBUF_INPUT(int, input_2, 50)
DOUBLEBUF_OUTPUT(int, output_1, 25)
DOUBLEBUF_OUTPUT(int, output_2, 25)

void compute(int buffer_id) {
  int* input_1 = DOUBLEBUF_BUFFER(buffer_id, input_1);
  int* input_2 = DOUBLEBUF_BUFFER(buffer_id, input_2);
  int* output_1 = DOUBLEBUF_BUFFER(buffer_id, output_1);
  int* output_2 = DOUBLEBUF_BUFFER(buffer_id, output_2);

  for(int i = 0; i < 25; ++i)
    output_1[i] = input_1[2*i] + input_2[2*i];
  for(int i = 0; i < 25; ++i)
    output_2[i] = input_1[2*i + 1] - input_2[2*i + 1] + i;
}


void main(void) {
  DOUBLEBUF_PORT_INPUTS {
    DOUBLEBUF_PORT_NAME(input_1),
    DOUBLEBUF_PORT_NAME(input_2),
  };
  DOUBLEBUF_PORT_OUTPUTS {
    DOUBLEBUF_PORT_NAME(output_1),
    DOUBLEBUF_PORT_NAME(output_2),
  };

  //DOUBLEBUF_COMPUTE_LOOP(compute, -1);
  DOUBLEBUF_COMPUTE_LOOP(compute, 10);
}
