#include <cstddef>

class SeqPool {

  char* guard_end;
  char* alloc_end;
  char* free_start;
  char* alloc_start;
  // guard_end < alloc_end <= free_start <= alloc_start

public:
  SeqPool(size_t pool_size, size_t max_alloc_sz);

  void* alloc(size_t bytes);

  ~SeqPool();
};
// vim: ts=2:sw=2
