int const input_1 input("input_1");
int const output_1 output("output_1");

struct packed4u8 {
  unsigned char byte[4];
};
struct packed4i8 {
  char byte[4];
};
struct packed2i16 {
  short word[2];
};

char volatile offset;

void main() {
  offset = fgetc(input_1);
  while(1) {
    int packed_bytes = fgetc(input_1);
    struct packed4u8* bytes = (struct packed4u8*)&packed_bytes;
    struct packed4i8 scaled_bytes;
    struct packed2i16 sums;

    for(int i = 0; i < 4; ++i) {
      scaled_bytes.byte[i] = (int)bytes->byte[i] - 128;
      scaled_bytes.byte[i] = (int)scaled_bytes.byte[i] + offset;
    }

    for(int i = 0; i < 2; ++i) {
      sums.word[i] = scaled_bytes.byte[2*i] + scaled_bytes.byte[2*i + 1];
    }

    fputc(*((int*)&sums), output_1);
  }
}
