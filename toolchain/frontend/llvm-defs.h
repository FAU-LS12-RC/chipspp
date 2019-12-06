#define LLVM_TOOLCHAIN
//TODO define the widths of these types in a TargetInfo in clang
//(requires recompilation)
//#define long long long
//#define char int

#define input(x) __attribute__((annotate("input " x)))
#define output(x) __attribute__((annotate("output " x)))

#define printf(...)

static inline int __attribute__((always_inline)) fgetc(int x) {
  int ret;
  asm volatile("read %0, *%1"
      : "=r" (ret)
      : "r" (x)
      :);
  return ret;
}
static inline void __attribute__((always_inline)) fputc(int val, int id) {
  asm volatile("write *%0, %1"
      :
      : "r" (id), "r" (val)
      :);
}
static inline void __attribute__((always_inline)) stop(void) {
  asm volatile("stop"
      :
      :
      :);
}
static inline unsigned long long __attribute__((always_inline)) timer_low(void) {
  unsigned long long ret;
  asm volatile("timer_low %0"
      : "=r" (ret)
      :
      :);
  return ret;
}
static inline unsigned long long __attribute__((always_inline)) timer_high(void) {
  unsigned long long ret;
  asm volatile("timer_high %0"
      : "=r" (ret)
      :
      :);
  return ret;
}
static inline void __attribute__((always_inline)) wait_clocks(
    int cycles_to_wait) {
  asm volatile("wait_clocks %0"
      : 
      : "r" (cycles_to_wait)
      :);
}
 

static inline float __attribute__((always_inline)) bits_to_float(unsigned n) {
  return *((float*)&n);
}
static inline unsigned __attribute__((always_inline)) float_to_bits(float x) {
  return *((unsigned*)&x);
}
union double_conversion {
  double x;
  unsigned long long n;
};
static inline double __attribute__((always_inline)) bits_to_double(
    unsigned long long n) {
  union double_conversion conv;
  conv.n = n;
  return conv.x;
//  return *((double*)&n);
}
static inline unsigned long long __attribute__((always_inline)) double_to_bits(
    double x) {
  union double_conversion conv;
  conv.x = x;
  return conv.n;
  //return *((unsigned long long*)&x);
}
//TODO implement
static void report(unsigned n) {
  asm volatile("a_lo_in %0\nreport"
      :
      : "r" (n)
      :); //TODO clobber "a");
}

//extern "C" void* memset(void* s, int c, int n) {
//  unsigned char* dest = (unsigned char*)s;
//  for(int i = 0; i < n; ++i)
//    dest[i] = c;
//  return s;
//}
