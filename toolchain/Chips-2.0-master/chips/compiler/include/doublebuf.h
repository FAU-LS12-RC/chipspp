//basic sequence:
//writewait_x
//write_y
//readwait_x
//read_y
//compute_x

#include <stddef.h>
#include <asyncio.h>

typedef struct {
  int io_id;
  size_t frame_size_in_words;
  void* front_buf;
  void* back_buf;
} doublebuf_port;

//typedef void (*doublebuf_compute_func)(int);

void* doublebuf_array(int buffer_id, doublebuf_port* port) {
  return buffer_id == 0 ? port->front_buf : port->back_buf;
}

//void compute_loop(struct doublebuf_port** inputs, size_t no_of_inputs,
//    struct doublebuf_port** outputs, size_t no_of_outputs,
//    doublebuf_compute_func compute) {
#define DOUBLEBUF_EXECUTE_COMPUTE_LOOP(                               \
    inputs, no_of_inputs,                                             \
    outputs, no_of_outputs,                                           \
    max_no_of_frames, compute)                                        \
do {                                                                  \
  int buffer_id = 1;                                                  \
  int i;                                                              \
  int first_run = 1;                                                  \
  int no_of_read_frames = 0;                                          \
                                                                      \
  for(i = 0; i < no_of_inputs; ++i) {                                 \
    asyncio_read(inputs[i]->io_id, inputs[i]->front_buf,              \
        inputs[i]->frame_size_in_words);                              \
    if(i < no_of_inputs - 1)                                          \
      asyncio_wait_read();                                            \
  }                                                                   \
                                                                      \
  while(1) {                                                          \
    asyncio_wait_write();                                             \
    if(!first_run) {                                                  \
      for(i = 0; i < no_of_outputs; ++i) {                            \
        asyncio_write(                                                \
            outputs[i]->io_id,                                        \
            doublebuf_array(buffer_id, outputs[i]),                   \
            outputs[i]->frame_size_in_words);                         \
        if(i < no_of_outputs - 1)                                     \
          asyncio_wait_write();                                       \
      }                                                               \
    }                                                                 \
                                                                      \
    asyncio_wait_read();                                              \
    no_of_read_frames += 1;                                           \
    if(no_of_read_frames < max_no_of_frames || max_no_of_frames == -1) {      \
      for(i = 0; i < no_of_inputs; ++i) {                             \
        asyncio_read(                                                 \
            inputs[i]->io_id,                                         \
            doublebuf_array(buffer_id, inputs[i]),                    \
            inputs[i]->frame_size_in_words);                          \
        if(i < no_of_inputs - 1)                                      \
          asyncio_wait_read();                                        \
      }                                                               \
    }                                                                 \
                                                                      \
    compute(!buffer_id);                                              \
                                                                      \
    buffer_id = !buffer_id;                                           \
    first_run = 0;                                                    \
  }                                                                   \
} while(0);

#define DOUBLEBUF_PORT_NAME(name) \
  (name##_port_def.io_id = name, &name##_port_def)
#define DOUBLEBUF_FRONTBUF(name) name##_data_1
#define DOUBLEBUF_BACKBUF(name) name##_data_2

#define DOUBLEBUF_INPUT(type, name, frame_size) \
  DOUBLEBUF_PORT(input, type, name, frame_size)
#define DOUBLEBUF_OUTPUT(type, name, frame_size) \
  DOUBLEBUF_PORT(output, type, name, frame_size)
#define DOUBLEBUF_PORT(kind, type, name, frame_size)         \
  int name kind(#name);                                      \
  type DOUBLEBUF_FRONTBUF(name)[frame_size];                 \
  type DOUBLEBUF_BACKBUF(name)[frame_size];                  \
  doublebuf_port name##_port_def = {                         \
    0, frame_size * sizeof(type) / 4,                        \
    DOUBLEBUF_FRONTBUF(name),                                \
    DOUBLEBUF_BACKBUF(name)                                  \
  };

#define DOUBLEBUF_PORT_INPUTS \
  doublebuf_port* doublebuf_inputs[] = 
#define DOUBLEBUF_PORT_OUTPUTS \
  doublebuf_port* doublebuf_outputs[] = 

#define DOUBLEBUF_COMPUTE_LOOP(compute_func, no_of_frames) \
  DOUBLEBUF_EXECUTE_COMPUTE_LOOP(                                 \
      doublebuf_inputs,                                           \
      sizeof(doublebuf_inputs) / sizeof(doublebuf_port*),         \
      doublebuf_outputs,                                          \
      sizeof(doublebuf_outputs) / sizeof(doublebuf_port*),        \
      no_of_frames,                                               \
      compute_func)

#define DOUBLEBUF_BUFFER(buffer_id, name) \
  (buffer_id == 0 ?                        \
    (void*)DOUBLEBUF_FRONTBUF(name) :     \
    (void*)DOUBLEBUF_BACKBUF(name))        \
