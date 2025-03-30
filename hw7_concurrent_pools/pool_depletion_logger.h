#include <cstddef>
#include <cstdint>

class PoolDepletionLogger {

  PoolDepletionLogger() { }

public:

  PoolDepletionLogger(PoolDepletionLogger &other) = delete;
  void operator=(const PoolDepletionLogger &) = delete;

  // protection area [from, to)
  static bool regHandler(size_t poolId, uintptr_t from, uintptr_t to);
  static void deregHandler(size_t poolId);

};
// vim: ts=2:sw=2
