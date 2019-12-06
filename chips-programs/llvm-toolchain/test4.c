const int input("input_1") input_1;
const int output("output_1") output_1;

struct cint {
  int re;
  int im;
};

void fft_custom(struct cint y[256]) {
  for(int i = 0; i < 200; ++i)
    y[i] = y[i + 5];
}

void main() {
  struct cint in_buffer[256];
  while(1) {
    for(int i = 0; i < 256; ++i) {
      in_buffer[i].re = fgetc(input_1);
      in_buffer[i].im = in_buffer[i].re;
      //report((int)in_buffer[i]);
    }
    fft_custom(in_buffer);

    for(int i = 0; i < 256; ++i) {
      fputc(in_buffer[i].re, output_1);
      fputc(in_buffer[i].im, output_1);
    }
  }
}
