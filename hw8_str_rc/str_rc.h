#ifndef STR_RC_GUARD_
#define STR_RC_GUARD_

#include <cassert>
#include <climits>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <functional>

class StrRC {
  union {
    struct {
      uintptr_t ptr : sizeof(uintptr_t) * CHAR_BIT - 1;
      bool is_unique : 1;
    };
    uintptr_t raw;
  };

  static constexpr char empty_string[] = "";

  inline uintptr_t get_ptr() {
    return ptr << 1;
  }

  inline void set_ptr(uintptr_t new_ptr) {
#ifdef STR_RC_DEBUG
    assert(!(new_ptr & 1));
#endif
    ptr = new_ptr >> 1;
  }

public:
  inline StrRC() {
    set_ptr((uintptr_t) 0);
    is_unique = false;
  }

  inline StrRC(char* str) {
    set_ptr((uintptr_t) str);
    is_unique = true;
  }

  inline StrRC(StrRC& oth) : ptr(oth.ptr) {
    is_unique = oth.is_unique = false;
  }

  inline StrRC& operator= (char* str) {
    StrRC tmp(str);
    this->swap(tmp);
    return *this;
  }

  inline StrRC& operator= (StrRC& oth) {
    StrRC tmp(oth);
    this->swap(tmp);
    return *this;
  }

  inline const char* operator* () {
    char* res = (char*) get_ptr();
    return (res ? res : empty_string);
  }

  inline void swap (StrRC& oth) {
    std::swap(raw, oth.raw);
#ifdef STR_RC_DEBUG
    std::swap(free_callback, oth.free_callback);
#endif
  }

#ifdef STR_RC_DEBUG
private:
  using callback_t = std::function<void(uintptr_t)>;
  callback_t free_callback;

public:
  inline void reg_free_callback(callback_t callback) {
    free_callback = std::move(callback);
  }
#endif

  ~StrRC() {

    if (is_unique) {
#ifdef STR_RC_DEBUG
      if (free_callback) {
        free_callback(get_ptr());
      }
#endif
      free((void*) get_ptr());
    }
  }

  friend std::ostream& operator<< (std::ostream&, StrRC);
};

inline std::ostream& operator<< (std::ostream& os, StrRC rc) {
  return os << "[str: \"" << *rc << "\"" << (rc.is_unique ? ", unique]" : "]");
}

#endif

// vim: ts=2:sw=2
