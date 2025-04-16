#include <csetjmp>
#include <iostream>

#include <cstdint>
#include <csignal>
#include <cassert>
#include <optional>

struct sigaction prev_handler;
std::optional<uintptr_t> target = {};
sigjmp_buf buf;

// Should be reentrant
void safe_read_handler(int sig, siginfo_t *info, void *ucontext) {
  assert(sig == SIGSEGV);

  uintptr_t mem_hit = (uintptr_t) info->si_addr;
  if (target.has_value() && *target == mem_hit) {
    siglongjmp(buf, 1);
  }

  if (prev_handler.sa_handler == SIG_DFL) {
    sigaction(sig, &prev_handler, NULL);
    raise(sig);
  } else if (prev_handler.sa_handler != SIG_IGN) {
    prev_handler.sa_handler(sig);
  }
}

inline std::optional<uint8_t> safe_read_uint8t(const uint8_t* p) {
  std::optional<uint8_t> res;

  if (!sigsetjmp(buf, 1)) {
    struct sigaction cur_handler = {
      .sa_mask = SA_NOMASK,
      .sa_flags = SA_SIGINFO,
    };
    cur_handler.sa_sigaction = safe_read_handler,
    assert(sigaction(SIGSEGV, &cur_handler, &prev_handler) == 0);
    target = (uintptr_t) p;
    res = *p;
  } else {
    res = std::nullopt;
  }

  // dereg handler
  assert(sigaction(SIGSEGV, &prev_handler, NULL) == 0);
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

  // Testing with external handler
  struct sigaction handler_to_restore;
  if (!setjmp(test_buf)) {
    struct sigaction test_handler = {
      .sa_mask = SA_NOMASK,
      .sa_flags = SA_SIGINFO,
    };
    test_handler.sa_sigaction = set_flag_handler;
    assert(sigaction(SIGSEGV, &test_handler, &handler_to_restore) == 0);
    test_flag = 0;
    assert(safe_read_uint8t((uint8_t*) 0) == std::nullopt);
    assert(safe_read_uint8t((uint8_t*)&n) == 5);
    assert(test_flag == 0); // external handler not called yet
    // External handler can catch segfault after usage safe_read_uint8_t
    *((uint8_t*)0x7000) = 0;
  } else {
    assert(sigaction(SIGSEGV, &handler_to_restore, NULL) == 0);
    assert(test_flag == 1);
  }

  std::cout << "Tests passed\n";
  return 0;
}
// vim: ts=2:sw=2
