#ifndef __ASYNCIO_H
#define __ASYNCIO_H

#include <stddef.h>

inline void __attribute__((always_inline)) asyncio_read(
        int io_id, void* dest, size_t size_in_words) {
  int id_and_length = io_id << 24 | size_in_words;
  asm volatile("asyncio_start_read %0, %1"
      : 
      : "r" (id_and_length), "r" (dest)
      :);
}
void asyncio_write(int io_id, void const* src, size_t size_in_words) {
  int id_and_length = io_id << 24 | size_in_words;
  asm volatile("asyncio_start_write %0, %1"
      : 
      : "r" (id_and_length), "r" (src)
      :);
}

void asyncio_wait_write() {
  asm volatile("asyncio_wait_write"
      : 
      :
      :);
}
void asyncio_wait_read() {
  asm volatile("asyncio_wait_read"
      : 
      :
      :);
}

#endif
