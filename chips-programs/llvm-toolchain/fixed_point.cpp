int input_1 input("input_1");
int output_1 output("output_1");

template<typename T, int integerBits, int fractionalBits>
class FixedPoint {
public:
  T value;
  using Self = FixedPoint<T, integerBits, fractionalBits>;

public:
  FixedPoint() : value(0) { }
  FixedPoint(T integer, T fractional) :
    value((integer << fractionalBits) | fractional) { }
  explicit FixedPoint(T value) :
    value(value) { }
  static FixedPoint<T, integerBits, fractionalBits> fromFloat(float value) {
    T integerPart = (T)value;
    if(value < 0.0f && integerPart != value)
      integerPart = (T)(value - 1.0f);
    float fractionalPart = value - integerPart;
    return Self(
        (integerPart << fractionalBits) |
        (T)(fractionalPart * (1 << fractionalBits)));
  }

  void operator+=(Self const& other) {
    value += other.value;
  }
  Self operator+(Self const& other) {
    Self ret(*this);
    ret += other;
    return ret;
  }
  void operator*=(Self const& other) {
    int result = value * other.value;
    value = result >> fractionalBits;
  }
  Self operator*(Self const& other) {
    Self ret(*this);
    ret *= other;
    return ret;
  }

  float scalingFactor() const {
    return 1.0f / (1 << fractionalBits);
  }

  float toFloat() const {
    float fractional = value & ((1 << fractionalBits) - 1);
    return (value >> fractionalBits) + fractional * scalingFactor();
  }
};

using FixedPoint8 = FixedPoint<short, 8, 8>;
void main() {
  FixedPoint8 sum(0, 0);
  while(true) {
    int integer = fgetc(input_1);
    FixedPoint8* factors = (FixedPoint8*)&integer;
    sum += factors[0] * factors[1];
    int output = *((unsigned short*)&sum.value);
    fputc(output, output_1);
  }
}
