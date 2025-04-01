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
    char* tmp = strdup(STR_A);
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
    StrRC a(strdup(STR_A));
    StrRC b(a);
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    b.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    assert(!strcmp(*a, STR_A));
    assert(!strcmp(*b, STR_A));
  }

  assert(!a_destructor_called);
  assert(!b_destructor_called);
}

inline void test_assingment_ptr() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;

  {
    StrRC a(strdup(STR_A));
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    a = strdup(STR_B);
    assert(a_destructor_called);
    a.reg_free_callback([&b_destructor_called] (uintptr_t) { b_destructor_called = true; });
    assert(!strcmp(*a, STR_B));
  }

  assert(b_destructor_called);
}

inline void test_assingment_rc() {
  bool a_destructor_called = false;
  bool b_destructor_called = false;

  {
    StrRC a(strdup(STR_A));
    a.reg_free_callback([&a_destructor_called] (uintptr_t) { a_destructor_called = true; });
    StrRC b(strdup(STR_B));
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

inline void test_bubble_sort() {
  size_t rc_frees = 0;
  constexpr int n = 5;
  {

    std::array<StrRC, n> arr = {
      strdup("caba"),
      strdup("aba"),
      strdup("cada"),
      strdup("bac"),
      strdup("abaab"),
    };

    for (auto& i : arr) {
      i.reg_free_callback([&rc_frees] (uintptr_t) { ++rc_frees; });
    }

    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < n; ++j) {
        if (strcmp(*arr[i], *arr[j]) > 0) {
          arr[i].swap(arr[j]);
        }
      }
    }
  }
  assert(rc_frees == n);
}

int main() {
  test_constructor_by_str();
  test_constructor_by_rc();
  test_assingment_ptr();
  test_assingment_rc();
  test_empty_init();
  test_bubble_sort();
  std::cout << "Tests passed\n";
  return 0;
}

// vim: ts=2:sw=2
