#include <cassert>
#include <csignal>
#include <cstring>
#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>

#include "pool_depletion_logger.h"

namespace {


struct sigaction cur_handler;
struct sigaction prev_handler;
struct GuardZone {
  uintptr_t start;
  uintptr_t end;
  size_t poolId;
};
std::vector<GuardZone> guard_zones;
constexpr char msg[] = "SeqPool exhausted! PoolId: ";
std::mutex reg_mut;

// Should be reentrant
void mhandler(int sig, siginfo_t *info, void *ucontext) {
  uintptr_t mem_hit = (uintptr_t) info->si_addr;

  for (const auto& zone : guard_zones) {
    if (zone.start <= mem_hit && mem_hit < zone.end) {
      write(STDERR_FILENO, msg, strlen(msg));
      // no std reent fun to convert int->string provided :(
      constexpr size_t buff_sz = 64;
      char buff[buff_sz];
      int x = zone.poolId;
      int len = 0;
      while (x != 0) {
        buff[len++] = x % 10 + '0';
        x /= 10;
      }
      buff[len] = '\n';
      buff[len + 1] = 0;
      for (int j = 0; j < len - j - 1; ++j) {
        std::swap(buff[j], buff[len - j - 1]);
      }
      write(STDERR_FILENO, buff, len + 1);
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

void PoolDepletionLogger::regHandler(size_t poolId, uintptr_t from, uintptr_t to) {
  std::lock_guard lock(reg_mut);
  if (guard_zones.empty()) {
    cur_handler.sa_flags = SA_SIGINFO;
    cur_handler.sa_sigaction = mhandler;
    sigemptyset(&cur_handler.sa_mask);
    assert(sigaction(SIGSEGV, &cur_handler, &prev_handler) == 0);
  }
  guard_zones.push_back({from, to, poolId});
}

void PoolDepletionLogger::deregHandler(size_t poolId) {
  std::lock_guard lock(reg_mut);
  auto zone = std::find_if(guard_zones.begin(), guard_zones.end(),
                           [&poolId] (auto zone) { return zone.poolId == poolId; });
  guard_zones.erase(zone);
  if (guard_zones.empty()) {
    sigaction(SIGSEGV, &prev_handler, NULL);
  }
}

// vim: ts=2:sw=2
