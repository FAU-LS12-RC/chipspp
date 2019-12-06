int const output_1 = output("output_1");

float const constant = 0.999698818F;

void main() {
    report((int)(constant * 1000.0f));

  fputc(0, output_1);
  fputc(0, output_1);
}
