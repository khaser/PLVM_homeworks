#define STR_RC_DEBUG
#include "str_rc.h"
#undef STR_RC_DEBUG

#include <cassert>
#include <cstring>
#include <iostream>

char STR_A[] = "first test string";
char STR_B[] = "second test str";

inline void test_constructor_by_str() {
  bool destructor_called = false;
  {
    char* tmp = STR_A;
    StrRC a(tmp);
    a.reg_free_callback([&destructor_called] (uintptr_t) { destructor_called = true; });
    assert(!strcmp(*a, tmp));
  }

  assert(destructor_called);
}

inline void test_constructor_by_rc() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;
  {
    StrRC a(STR_A);
    StrRC b(a);
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    b.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    assert(!strcmp(*a, STR_A));
    assert(!strcmp(*b, STR_A));
  }

  assert(!a_destructor_called);
  assert(!b_destructor_called);
}

inline void test_assignment_ptr() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;

  {
    StrRC a(STR_A);
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    a = STR_B;
    assert(a_destructor_called);
    a.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    assert(!strcmp(*a, STR_B));
  }

  assert(b_destructor_called);
}

inline void test_assignment_rc() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;

  {
    StrRC a(STR_A);
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    StrRC b(STR_B);
    b.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    a = b;
    assert(a_destructor_called && !b_destructor_called);
    assert(!strcmp(*a, STR_B));
  }

  assert(!b_destructor_called);
}

inline void test_empty_init() {
  bool destructor_called = false;

  {
    StrRC a;
    a.reg_free_callback([&destructor_called] (uintptr_t) { destructor_called = true; });
    assert(!strcmp(*a, ""));
  }

  assert(!destructor_called);
}

inline void test_self_assignment() {
  bool destructor_called = false;

  {
    StrRC a(STR_A);
    a.reg_free_callback([&destructor_called] (uintptr_t) { destructor_called = true; });
    a = a;
  }

  assert(destructor_called);
}

inline void test_bubble_sort() {
  size_t rc_frees = 0;
  constexpr int n = 5;
  {

    std::array<StrRC, n> arr = {
      "caba",
      "aba",
      "cada",
      "bac",
      "abaab",
    };

    for (auto& i : arr) {
      i.reg_free_callback([&rc_frees] (uintptr_t) { ++rc_frees; });
    }

    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < n; ++j) {
        if (strcmp(*arr[i], *arr[j]) > 0) {
          StrRC tmp = std::move(arr[i]);
          arr[i] = std::move(arr[j]);
          arr[j] = std::move(tmp);
        }
      }
    }
  }
  assert(rc_frees == n);
}

inline void test_move_constructor() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;
  {
    StrRC a(STR_A);
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    StrRC b(std::move(a));
    b.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    assert(!strcmp(*b, STR_A));
  }

  assert(!a_destructor_called);
  assert(b_destructor_called);
}

inline void test_move_assignment() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;

  {
    StrRC a(STR_A);
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    StrRC b(STR_B);
    b.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    b = std::move(a);
    assert(!a_destructor_called);
    assert(b_destructor_called);
    assert(!strcmp(*b, STR_A));
  }

  assert(a_destructor_called);
}

int main() {
  test_constructor_by_str();
  test_constructor_by_rc();
  test_assignment_ptr();
  test_assignment_rc();
  test_empty_init();
  test_self_assignment();
  test_move_assignment();
  test_move_constructor();
  test_bubble_sort();
  std::cout << "Tests passed\n";
  return 0;
}

// vim: ts=2:sw=2
