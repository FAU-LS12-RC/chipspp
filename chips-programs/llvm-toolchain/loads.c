int output_1 output("output_1");

int x = 3;
int* xp = &x;
int** xpp = &xp;
int*** xppp = &xpp;

int get(int*** xppp) {
  return ***xppp;
}

void main(void) {
  fputc(get(xppp), output_1);
}
