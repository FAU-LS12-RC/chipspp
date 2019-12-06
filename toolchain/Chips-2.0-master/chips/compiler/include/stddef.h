#ifndef __STDDEF_H
#define __STDDEF_H

#ifdef LLVM_TOOLCHAIN
typedef unsigned int size_t;
#define NULL 0
#else
#define NULL (int*)0xffffffffu
#endif

#endif
