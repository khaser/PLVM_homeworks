#include <cerrno>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/mman.h>

#include <iostream>
#include <vector>

#include "lock_free_seq_pool.h"
#include "pool_depletion_logger.h"

std::atomic_size_t LockFreeSeqPool::poolFreeId = 0;

LockFreeSeqPool::LockFreeSeqPool(size_t pool_size, size_t max_alloc_sz) {
  assert(pool_size >= max_alloc_sz);
  long pg_size = sysconf(_SC_PAGE_SIZE);
  size_t pgs_to_protect = (max_alloc_sz + pg_size - 1) / pg_size;
  size_t pgs_to_alloc = (pool_size + pg_size - 1) / pg_size + pgs_to_protect;
  guard_end = (intptr_t) mmap(0, pgs_to_alloc * pg_size,
                           PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN, 0, 0);
  assert(guard_end != -1);
  assert(mprotect((void*) guard_end, pgs_to_protect * pg_size, PROT_NONE | PROT_GROWSDOWN) == 0);

  alloc_end = guard_end + pgs_to_protect * pg_size;
  alloc_start = guard_end + pgs_to_alloc * pg_size;
  free_start.store(alloc_start, std::memory_order_relaxed);

  poolId = poolFreeId.fetch_add(1, std::memory_order_relaxed) + 1;

  assert(PoolDepletionLogger::regHandler(poolId, guard_end, alloc_end) && "CNT_POOL_QUOTA exhausted");
}

LockFreeSeqPool::~LockFreeSeqPool() {
  munmap((void*) guard_end, alloc_start - guard_end);
  PoolDepletionLogger::deregHandler(poolId);
}

// vim: ts=2:sw=2
