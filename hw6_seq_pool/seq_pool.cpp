#include <cerrno>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/mman.h>

#include <iostream>
#include <vector>

#include "seq_pool.h"

namespace {
struct sigaction cur_handler;
struct sigaction prev_handler;
std::vector<std::pair<char*, char*>> guard_zones;
static bool isHandlerRegistred;

class PoolDepletionLogger {

  PoolDepletionLogger() { }

  static PoolDepletionLogger* ptr;
  static constexpr char msg[] = "SeqPool exhausted!\n";

public:

  PoolDepletionLogger(PoolDepletionLogger &other) = delete;
  void operator=(const PoolDepletionLogger &) = delete;

  static void regHandler() {
    if (!isHandlerRegistred) {
      cur_handler.sa_flags = SA_SIGINFO;
      cur_handler.sa_sigaction = handler;
      sigemptyset(&cur_handler.sa_mask);
      assert(sigaction(SIGSEGV, &cur_handler, &prev_handler) == 0);
    }
  }

  // Should be reentrant
  static void handler(int sig, siginfo_t *info, void *ucontext) {
    char* mem_hit = (char*) info->si_addr;

    for (const auto& zone : guard_zones) {
      if (zone.first <= mem_hit && mem_hit < zone.second) {
        write(STDERR_FILENO, msg, strlen(msg));
        break;
      }
    }

    if (prev_handler.sa_handler == SIG_DFL) {
      sigaction(sig, &prev_handler, NULL);
      raise(sig);
    } else if (prev_handler.sa_handler != SIG_IGN) {
      prev_handler.sa_handler(sig);
    }
  }

};

PoolDepletionLogger* PoolDepletionLogger::ptr;
}

SeqPool::SeqPool(size_t pool_size, size_t max_alloc_sz) {
  assert(pool_size >= max_alloc_sz);
  long pg_size = sysconf(_SC_PAGE_SIZE);
  size_t pgs_to_protect = (max_alloc_sz + pg_size - 1) / pg_size;
  size_t pgs_to_alloc = (pool_size + pg_size - 1) / pg_size + pgs_to_protect;
  guard_end = (char*) mmap(0, pgs_to_alloc * pg_size,
                           PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_GROWSDOWN, 0, 0);
  assert((ssize_t) guard_end != -1);
  assert(mprotect(guard_end, pgs_to_protect * pg_size, PROT_NONE | PROT_GROWSDOWN) == 0);

  alloc_end = guard_end + pgs_to_protect * pg_size;
  alloc_start = guard_end + pgs_to_alloc * pg_size;
  free_start = alloc_start;

  guard_zones.push_back({guard_end, alloc_end});
  PoolDepletionLogger::regHandler();
}

void* SeqPool::alloc(size_t bytes) {
  return free_start -= bytes;
}

SeqPool::~SeqPool() {
  munmap(guard_end, alloc_start - guard_end);
}

// vim: ts=2:sw=2
