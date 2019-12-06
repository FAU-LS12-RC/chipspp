#ifndef __STDIO_H
#define __STDIO_H

unsigned stdin = 0;
unsigned stdout = 0;

//typedef long long i64;
//typedef unsigned long long u64;
//TODO use stdint
#define i64 long long
#define u64 unsigned long long

///stdio.h
///-------
///
///In contrast to the C standard, `fputc` and `fgetc` are built-in functions, you
///do not need to include `stdio.h` to use them.
///
///The globals `stdin` and `stdout` should be set to an input or output by the user.
///


///The `fputs` function prints `string` to the output `handle`.
///
///.. code-block:: c
///
///        void fputs(unsigned string[], unsigned handle);
///
void fputs(unsigned string[], unsigned handle){
        unsigned i=0;
        while(string[i]){
                fputc(string[i], handle);
                i++;
        }
}

///The `fgets` function reads a line, up to `maxlength` characters, or a line end
///from the input `handle`. The string will be null terminated. `maxlength`
///includes the null character.
///
///.. code-block:: c
///
///        void fgets(unsigned string[], unsigned maxlength, unsigned handle);
///

void fgets(unsigned string[], unsigned maxlength, unsigned handle){
        unsigned c;
        unsigned i=0;
        while(1){
                c = fgetc(handle);
                string[i] = c;
                i++;
                if(c == '\n') break;
                if(i == maxlength-1) break;
        }
        string[i] = 0;
}

///The `puts` function prints `string` to stdout.
///
///.. code-block:: c
///
///        void puts(unsigned string[]);
///

void gets(unsigned string[], unsigned maxlength){
        fgets(string, maxlength, stdin);
}

///The `gets` function reads a line, up to `maxlength` characters, or a line end
///from stdin. The string will be null terminated. `maxlength`
///includes the null character.
///
///.. code-block:: c
///
///        void gets(unsigned string[], unsigned maxlength);
///

void puts(unsigned string[]){
        fputs(string, stdout);
}

///The `getc` returns a single character from stdin.
///
///.. code-block:: c
///
///        unsigned long getc();
///

unsigned getc(){
        return fgetc(stdin);
}

///The `putc` writes a single character to stdout.
///
///.. code-block:: c
///
///        void putc(unsigned c);
///

void putc(unsigned c){
        fputc(c, stdout);
}

/*Non-Standard Extensions*/

void fput_double(double d, unsigned handle){
    u64 l;
    unsigned low, high;
    l = double_to_bits(d);
    low = l & 0xffffffffu;
    high = l >> 32;
    fputc(low, handle);
    fputc(high, handle);
}

double fget_double(unsigned handle){
    unsigned low, high;
    u64 l;
    double d;
    low = fgetc(handle);
    high = fgetc(handle);
    l = ((i64) high) << 32;
    l |= low;
    d =  bits_to_double(l);
    return d;
}

void fput_float(float d, unsigned handle){
    fputc(float_to_bits(d), handle);
}

float fget_float(unsigned handle){
    return bits_to_float(fgetc(handle));
}

void fput_int(int d, unsigned handle){
    fputc(d, handle);
}

int fget_int(unsigned handle){
    return fgetc(handle);
}

void fput_long(i64 d, unsigned handle){
    int low, high;
    i64 l = d;
    low = l & 0xffffffffu;
    high = l >> 32;
    fputc(low, handle);
    fputc(high, handle);
}

i64 fget_long(unsigned handle){
    int low, high;
    i64 l;
    low = fgetc(handle);
    high = fgetc(handle);
    l = ((i64) high) << 32;
    l |= low;
    return l;
}

#ifdef LLVM_TOOLCHAIN
void get_bytes(char* dest_bytes, int input) {
    int* dest = (int*)dest_bytes;
    *dest = fgetc(input);
}
void put_bytes(unsigned char* src_bytes, int output) {
    int* src = (int*)src_bytes;
    fputc(*src, output);
    //report(*src);
//    int n = (src_bytes[3] << 24) | (src_bytes[2] << 16) | (src_bytes[1] << 8) | src_bytes[0];
//    fputc(n, output);
}
#endif

#ifdef IN_MEM_STDIO
#include <stddef.h>
#include <string.h>

#ifndef IN_MEM_PREFIX
#  define IN_MEM_PREFIX
#endif
#define concat2(x, y) x##y
#define concat(x, y) concat2(x, y)
#define inmem_name(s) concat(IN_MEM_PREFIX, s)
#define FILE concat(IN_MEM_PREFIX, FILE)
#define fpos_t concat(IN_MEM_PREFIX, fpos_t)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef size_t fpos_t;
typedef void FILE;
struct InMemoryFile {
  unsigned char* buf;
  size_t len;
  size_t pos;
  int eof;
};

FILE* memio_init(struct InMemoryFile* file_struct, unsigned char* buf,
        size_t len) {
//  unsigned char* buf = malloc(len);
//  if(!buf)
//    return NULL;
  file_struct->buf = buf;
  file_struct->len = len;
  file_struct->pos = 0;
  file_struct->eof = 0;
  return (FILE*)file_struct;
}

static size_t min(size_t a, size_t b) {
  if(a < b)
    return a;
  return b;
}

size_t inmem_name(fread)(void* dest, size_t size, size_t n, FILE* file_ptr) {
  struct InMemoryFile* file = (struct InMemoryFile*)file_ptr;
  //assert(size == 1)
  size_t ret = min(file->len - file->pos, n);
  memcpy(dest, file->buf + file->pos, ret);
  file->pos += ret;
  if(ret == 0)
    file->eof = 1;
  return ret;
}
size_t inmem_name(fwrite)(void const* src, size_t size, size_t n,
    FILE* file_ptr) {
  struct InMemoryFile* file = (struct InMemoryFile*)file_ptr;
  //assert(size == 1)
  memcpy(file->buf + file->pos, src, n);
  file->pos += n;
  return n;
}
int inmem_name(fseek)(FILE* file_ptr, long offset, int whence) {
  struct InMemoryFile* file = (struct InMemoryFile*)file_ptr;
  if(whence == SEEK_SET)
    file->pos = offset;
  else if(whence == SEEK_CUR)
    file->pos += offset;
  else
    file->pos = file->len + offset;
  return 0;
}
int inmem_name(fgetpos)(FILE* file_ptr, fpos_t* pos) {
  struct InMemoryFile* file = (struct InMemoryFile*)file_ptr;
  *pos = file->pos;
  return 0;
}
long inmem_name(ftell)(FILE* file_ptr) {
  struct InMemoryFile* file = (struct InMemoryFile*)file_ptr;
  return file->pos;
}
int inmem_name(feof)(FILE* file_ptr) {
  struct InMemoryFile* file = (struct InMemoryFile*)file_ptr;
  return file->eof;
}

void fill_memio_buf(struct InMemoryFile* dest, int src) {
  for(size_t i = 0; i < dest->len; i += 4) {
    get_bytes(dest->buf + i, src);
  }
}
void output_memio_buf(int dest, struct InMemoryFile* src) {
  for(size_t i = 0; i < src->len; i += 4) {
    put_bytes(src->buf + i, dest);
  }
}

#undef FILE
#undef fpos_t
#endif

#endif

