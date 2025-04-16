#include <cstddef>
#include <cstdint>

class SeqPool {

  intptr_t guard_end;
  intptr_t alloc_end;
  intptr_t free_start;
  intptr_t alloc_start;
  // guard_end < alloc_end <= free_start <= alloc_start

  static std::atomic_size_t poolFreeId;
  size_t poolId;

public:
  SeqPool(size_t pool_size, size_t max_alloc_sz);

  inline void* alloc(size_t bytes) {
    return (void*) (free_start -= bytes);
  }

  ~SeqPool();
};
// vim: ts=2:sw=2
