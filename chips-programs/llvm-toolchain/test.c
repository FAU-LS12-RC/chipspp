int const output_1 output("output_1");

float const constant = 0.999698818F;

//void process(struct creal y[], int k, int nHalf) {
//  float factor = 1.0f;
//  for(int i = 0; i < k; ++i) {
//    y[i].re += i * factor;
//    y[i].im += 1.0f;
//  }
//}

void main(void) {
    report((int)((&constant)[0] * 1000.0f));

//  for(int i = 0; i < 192; ++i)
//    report((int)(twiddleTable[i] * 1000.0f));
//
////  process(array, 256, 0);
//  for(int i = 0; i < 256; ++i) {
////    report(i);
////    report((int)array[i].re);
//    fput_float(1.0f, output_1);
//    fput_float(1.0f, output_1);
//  fput_float(0.0f, output_1);
  fputc(0, output_1);
  fputc(0, output_1);
}
