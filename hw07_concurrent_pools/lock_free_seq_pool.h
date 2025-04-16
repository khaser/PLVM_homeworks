#include <atomic>
#include <cstddef>
#include <cstdint>

class LockFreeSeqPool {

  intptr_t guard_end;
  intptr_t alloc_end;
  std::atomic_intptr_t free_start;
  intptr_t alloc_start;
  // guard_end < alloc_end <= free_start <= alloc_start
  size_t poolId;

  static std::atomic_size_t poolFreeId;

public:
  LockFreeSeqPool(size_t pool_size, size_t max_alloc_sz);

  inline void* alloc(size_t bytes) {
    return (void*) (free_start.fetch_sub(bytes, std::memory_order_acq_rel) - bytes);
  }

  ~LockFreeSeqPool();
};
// vim: ts=2:sw=2
