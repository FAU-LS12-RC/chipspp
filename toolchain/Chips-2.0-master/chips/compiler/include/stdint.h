#ifndef __STDINT_H
#define __STDINT_H

#ifdef LLVM_TOOLCHAIN
typedef unsigned long long uint64_t;
typedef long long int64_t;

typedef unsigned int uint32_t;
typedef int int32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;
#else
#define uint64_t unsigned long
#define int64_t long
#endif

#endif
