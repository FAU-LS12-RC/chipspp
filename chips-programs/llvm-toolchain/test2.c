int const input_1 input("input_1");
int const output_1 output("output_1");

struct point {
  int x;
  int y;
};

void main(void) {
  struct point p[256];
  int n = fgetc(input_1);
  for(int i = 0; i < 256; ++i) {
    if(i == n)
      p[i].x = fgetc(input_1);
    else
      p[i].x = 0;
    p[i].y = 0;
  }
//  p[i].x = fgetc(input_1);
//  p[i].y = fgetc(input_1);
  fputc(p[n].x, output_1);
}
