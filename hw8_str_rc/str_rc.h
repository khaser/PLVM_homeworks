#ifndef STR_RC_GUARD_
#define STR_RC_GUARD_

#include <cstring>
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

  uintptr_t get_ptr() const {
    return ptr << 1;
  }

  void set_ptr(uintptr_t new_ptr) {
#ifdef STR_RC_DEBUG
    assert(!(new_ptr & 1));
#endif
    ptr = new_ptr >> 1;
  }

  void swap(StrRC& oth) {
    std::swap(raw, oth.raw);
#ifdef STR_RC_DEBUG
    std::swap(free_callback, oth.free_callback);
#endif
  }

public:
  StrRC() {
    set_ptr((uintptr_t) 0);
    is_unique = false;
  }

  StrRC(const char* str) {
    set_ptr((uintptr_t) strdup(str));
    is_unique = true;
  }

  StrRC(StrRC& oth) : ptr(oth.ptr) {
    is_unique = oth.is_unique = false;
  }

  StrRC(StrRC&& oth) : ptr(oth.ptr), is_unique(oth.is_unique), free_callback(oth.free_callback) {
    oth.is_unique = false;
  }

  StrRC& operator= (const char* str) {
    StrRC tmp((char*) strdup(str));
    this->swap(tmp);
    return *this;
  }

  StrRC& operator= (StrRC& oth) {
    if (this == &oth) return *this;
    StrRC tmp(oth);
    this->swap(tmp);
    return *this;
  }

  StrRC& operator= (StrRC&& oth) {
    if (this == &oth) return *this;
    StrRC tmp(std::move(oth));
    this->swap(tmp);
    return *this;
  }

  const char* operator* () const {
    char* res = (char*) get_ptr();
    return (res ? res : "");
  }

#ifdef STR_RC_DEBUG
private:
  using callback_t = std::function<void(uintptr_t)>;
  callback_t free_callback;

public:
  void reg_free_callback(callback_t callback) {
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

  friend std::ostream& operator<< (std::ostream&, const StrRC&);
};

std::ostream& operator<< (std::ostream& os, const StrRC& rc) {
  return os << "[str: \"" << *rc << "\"" << (rc.is_unique ? ", unique]" : "]");
}

#endif

// vim: ts=2:sw=2
