#include <atomic>
#include <cassert>
#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include <algorithm>

#include "pool_depletion_logger.h"

#define CNT_POOLS_QUOTA 64

namespace {

struct sigaction cur_handler;
struct sigaction prev_handler;
struct GuardZone {
  uintptr_t start;
  uintptr_t end;
  size_t poolId;
};
std::atomic<std::shared_ptr<GuardZone>> guard_zones[CNT_POOLS_QUOTA];
int activeZones = 0;
constexpr char msg[] = "SeqPool exhausted! PoolId (last 4 digits): ";
std::mutex reg_mut;

// Should be reentrant
void mhandler(int sig, siginfo_t *info, void *ucontext) {
  uintptr_t mem_hit = (uintptr_t) info->si_addr;

  for (size_t i = 0; i < CNT_POOLS_QUOTA; ++i) {
    auto zone = guard_zones[i].load(std::memory_order_relaxed);
    if (zone == 0) continue;
    if (zone->start <= mem_hit && mem_hit < zone->end) {
      write(STDERR_FILENO, msg, strlen(msg));
      int x = zone->poolId;
      // no std reent fun to convert int->string provided :(
      int64_t s = 0x0a30303030 | (x / 1000)
                               | (x / 100 % 10) << 8
                               | (x / 10 % 10) << 16
                               | (x % 10) << 24;
      write(STDERR_FILENO, (char*) &s, 5);
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

} // anon namespace

bool PoolDepletionLogger::regHandler(size_t poolId, uintptr_t from, uintptr_t to) {
  std::lock_guard lock(reg_mut);
  assert(activeZones <= CNT_POOLS_QUOTA);
  if (activeZones == CNT_POOLS_QUOTA) return false;

  for (size_t i = 0; i < CNT_POOLS_QUOTA; ++i) {
    auto zone = guard_zones[i].load(std::memory_order_acquire);
    if (zone == 0) {
      std::shared_ptr<GuardZone> new_zone(new GuardZone {from, to, poolId});
      guard_zones[i].store(new_zone, std::memory_order_release);
      if (activeZones++ == 0) {
        cur_handler.sa_flags = SA_SIGINFO;
        cur_handler.sa_sigaction = mhandler;
        sigemptyset(&cur_handler.sa_mask);
        assert(sigaction(SIGSEGV, &cur_handler, &prev_handler) == 0);
      }
      return true;
    }
  }

  assert(0);
}

void PoolDepletionLogger::deregHandler(size_t poolId) {
  std::lock_guard lock(reg_mut);

  for (size_t i = 0; i < CNT_POOLS_QUOTA; ++i) {
    auto zone = guard_zones[i].load(std::memory_order_acquire);
    if (zone && zone->poolId == poolId) {
      guard_zones[i].store({}, std::memory_order_release);
      if (--activeZones == 0) {
        sigaction(SIGSEGV, &prev_handler, NULL);
      }
      return;
    }
  }

  assert(0 && "Can't find pool with poolId on handler deregistration");
}

// vim: ts=2:sw=2
