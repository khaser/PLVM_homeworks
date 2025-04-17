#include <csetjmp>
#include <iostream>

#include <cstdint>
#include <csignal>
#include <cassert>
#include <optional>

struct sigaction prev_sigsegv_handler;
struct sigaction prev_sigbus_handler;
std::optional<uintptr_t> target = {};
sigjmp_buf buf;

// Should be reentrant
void safe_read_handler(int sig, siginfo_t *info, void *ucontext) {
  uintptr_t mem_hit = (uintptr_t) info->si_addr;
  if (target.has_value() && *target == mem_hit) {
    siglongjmp(buf, 1);
  }

  if (sig == SIGSEGV) {
    prev_sigsegv_handler.sa_sigaction(sig, info, ucontext);
  } else if (sig == SIGBUS) {
    prev_sigbus_handler.sa_sigaction(sig, info, ucontext);
  } else {
    exit(1);
  }
}

inline std::optional<uint8_t> safe_read_uint8t(const uint8_t* p) {
  std::optional<uint8_t> res;

#ifdef __x86_64__
  constexpr unsigned long long low_seg = (1ull << 47) - 1;
  constexpr unsigned long long high_seg = ~low_seg;
  // Linux kernel doesn't provide correct siginfo_t for handler
  // in this address range
  if (low_seg < (uintptr_t) p && (uintptr_t) p < high_seg) {
    return std::nullopt;
  }
#endif

  if (!sigsetjmp(buf, 1)) {
    struct sigaction cur_handler = {
      .sa_mask = SA_NOMASK,
      .sa_flags = SA_SIGINFO,
    };
    cur_handler.sa_sigaction = safe_read_handler;
    int ret = sigaction(SIGSEGV, &cur_handler, &prev_sigsegv_handler)
              || sigaction(SIGBUS, &cur_handler, &prev_sigbus_handler);
    assert(ret == 0);
    target = (uintptr_t) p;
    res = *p;
  } else {
    res = std::nullopt;
  }

  // dereg handlers
  int ret = sigaction(SIGSEGV, &prev_sigsegv_handler, NULL)
            || sigaction(SIGBUS, &prev_sigbus_handler, NULL);
  assert(ret == 0);
  return res;
}

// Testing
sigjmp_buf test_buf;
char test_flag;

void set_flag_handler(int sig, siginfo_t *info, void *ucontext) {
  test_flag++;
  longjmp(test_buf, 1);
}

int main() {
  int n = 5;

  // Simple tests
  assert(safe_read_uint8t((uint8_t*) 0) == std::nullopt);
  assert(safe_read_uint8t((uint8_t*)&n) == 5);
  assert(safe_read_uint8t((uint8_t*) 0x4235) == std::nullopt);

  assert(safe_read_uint8t((uint8_t*) 0xFFF00000FFFF0000) == std::nullopt);
  assert(safe_read_uint8t((uint8_t*) 0xFFFF'7fff'ffff'ffff) == std::nullopt);
  assert(safe_read_uint8t((uint8_t*) 0xFFFF'8000'0000'0000) == std::nullopt);
  assert(safe_read_uint8t((uint8_t*) 0x0000'7800'0000'0000) == std::nullopt);

  // Testing with external handler
  struct sigaction handler_to_restore;
  if (!setjmp(test_buf)) {
    struct sigaction test_handler = {
      .sa_mask = SA_NOMASK,
      .sa_flags = SA_SIGINFO,
    };
    test_handler.sa_sigaction = set_flag_handler;
    sigaction(SIGSEGV, &test_handler, &handler_to_restore);
    test_flag = 0;
    assert(safe_read_uint8t((uint8_t*) 0) == std::nullopt);
    assert(safe_read_uint8t((uint8_t*)&n) == 5);
    assert(safe_read_uint8t((uint8_t*)&n) == 5);
    assert(test_flag == 0); // external handler not called yet
    // External handler can catch segfault after usage safe_read_uint8_t
    *((uint8_t*)0x7000) = 0;
  } else {
    sigaction(SIGSEGV, &handler_to_restore, NULL);
    assert(test_flag == 1);
  }

  std::cout << "Tests passed\n";
  return 0;
}
// vim: ts=2:sw=2
