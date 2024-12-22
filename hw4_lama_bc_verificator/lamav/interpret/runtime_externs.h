#ifndef RUNTIME_EXTERNS_H
#define RUNTIME_EXTERNS_H

extern "C" {

#include "../../runtime/gc.h"

  int   Lread ();
  int   Lwrite (int n);
  int   Llength (void *p);
  void* Lstring (void *p);
  int   LtagHash (const char *s);

  void* Bstring(const void *p);
  void* Bsta (void *v, int i, void *x);
  void* Belem (void *p, int i);
  int   Btag (void *d, int t, int n);
  int   Barray_patt (void *d, int n);

  void  Bmatch_failure (void *v, char *fname, int line, int col);
  int   Bstring_patt (void *x, void *y);
  int   Bclosure_tag_patt (void *x);
  int   Bboxed_patt (void *x);
  int   Bunboxed_patt (void *x);
  int   Barray_tag_patt (void *x);
  int   Bstring_tag_patt (void *x);
  int   Bsexp_tag_patt (void *x);

}

/* Following routines copy-pasted from runtime,
 * but takes arguments from local virtual stack instead of va_args (host stack) */

#include "vstack.h"

namespace {

inline void* Barray(int n) {
  int i;
  data* r = (data *)alloc_array(n);

  for (i = n - 1; i >= 0; --i) {
    ((int*)r->contents)[i] = POP();
  }

  return r->contents;
}

inline void* Bsexp(int n, int tag) {
  int fields_cnt = n;
  data* r = (data *)alloc_sexp(fields_cnt);
  ((sexp *)r)->tag = 0;

  for (int i = n; i > 0; --i) {
    ((int *)r->contents)[i] = POP();
  }

  ((sexp *)r)->tag = tag;

  return (int *)r->contents;
}

inline void* Bclosure(int n, void* entry) {
  data* r = (data*) alloc_closure(n + 1);
  ((void **)r->contents)[0] = entry;

  for (int i = n; i >= 1; --i) {
    ((int *)r->contents)[i] = POP();
  }

  return r->contents;
}

} // anon namespace

#endif
