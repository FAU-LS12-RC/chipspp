#include <stdlib.h>

#ifndef HEAP_SIZE
#define HEAP_SIZE 0
#endif

#define WORD_SIZE(n) ((n + 3) / 4)

#define CHIPS 0
#define AVR 1
//#define MALLOC_IMPL CHIPS
#define MALLOC_IMPL AVR

extern int _memory[WORD_SIZE(HEAP_SIZE)];

#if MALLOC_IMPL == CHIPS

#define ALLOCATED 0x20000
#define BLOCK_SIZE 0x1ffff
#define LAST_BLOCK 0x40000

static int _frees_without_clean;

void _malloc_init(void) {
  _memory[0] = LAST_BLOCK;
  _memory[0] |= WORD_SIZE(HEAP_SIZE);
}

static void* _alloc(int);
static void _clean(void);
void* malloc(size_t size) {
  void* ret;
  ret = _alloc(size);
  if(ret == NULL && _frees_without_clean > 0) {
    _clean();
    ret = _alloc(size);
  }
  return ret;
}

static void* _alloc(int size) {
    int block = 0;
    int block_size;
    int last_block;

    if(size == 0) return NULL;

    size = WORD_SIZE(size);

    while(1) {
        block_size = _memory[block] & BLOCK_SIZE;
        last_block = _memory[block] & LAST_BLOCK;
        if(block_size >= size && !(_memory[block] & ALLOCATED))
            break;
        if(last_block)
            return NULL;
        block += block_size + 1;
    }

    _memory[block] &= ~(ALLOCATED | BLOCK_SIZE);
    _memory[block] |= ALLOCATED | size;
    if(block_size > size) {
        _memory[block] &= ~LAST_BLOCK;
        _memory[block + size + 1] = block_size - size - 1;
        if(last_block)
          _memory[block + size + 1] |= LAST_BLOCK;
    }

    return &_memory[block] + 1;
}

/*Merge non-contiguous blocks*/
static void _clean(){
    int block = 0, next_block;
    int block_size, next_block_size;

    _frees_without_clean = 0;

    while(1) {
        if(_memory[block] & LAST_BLOCK)
            break;

        block_size = _memory[block] & BLOCK_SIZE;
        next_block = block + block_size + 1;
        if(_memory[block] & ALLOCATED) {
            block = next_block;
            continue;
        }

        while(1) {
            next_block_size = _memory[next_block] & BLOCK_SIZE;
            if(_memory[next_block] & ALLOCATED)
                break;
            _memory[block] += next_block_size + 1;
            if(_memory[next_block] & LAST_BLOCK) {
                _memory[block] |= LAST_BLOCK;
                break;
            }
            next_block += next_block_size + 1;
        }
        block = next_block;
    }
}

void free(void* block_in) {
    int* block = block_in;
    int header = *(block - 1);
    *(block - 1) = (header & ~ALLOCATED);
    _frees_without_clean += 1;
    if(_frees_without_clean > 100)
      _clean();
}

#elif MALLOC_IMPL == AVR

#include <optional/lib_malloc_avr.c>

#endif

void* realloc(void* ptr, int size){
    int* newptr = malloc(size);
    int* ints = ptr;
    int i;
    for(i = 0; i < WORD_SIZE(size); i++)
        newptr[i] = ints[i];
    free(ptr);
    return newptr;
}

void* calloc(int size, int length) {
    void* ptr = malloc(size*length);
    int* ints = ptr;
    int i;
    for(i=0; i < WORD_SIZE(size * length); i++)
        ints[i] = 0;
    return ptr;
}
