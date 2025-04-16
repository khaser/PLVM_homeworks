#ifndef RUNTIME_EXTERNS_H
#define RUNTIME_EXTERNS_H

extern int   Lread ();
extern int   Lwrite (int n);
extern int   Llength (void *p);
extern void* Lstring (void *p);
extern int   LtagHash (const char *s);

extern void* Bstring(const void *p);
extern void* Bsta (void *v, int i, void *x);
extern void* Belem (void *p, int i);
extern int   Btag (void *d, int t, int n);
extern int   Barray_patt (void *d, int n);

extern void  Bmatch_failure (void *v, char *fname, int line, int col);
extern int   Bstring_patt (void *x, void *y);
extern int   Bclosure_tag_patt (void *x);
extern int   Bboxed_patt (void *x);
extern int   Bunboxed_patt (void *x);
extern int   Barray_tag_patt (void *x);
extern int   Bstring_tag_patt (void *x);
extern int   Bsexp_tag_patt (void *x);

/* Following routines copy-pasted from runtime,
 * but takes arguments from local virtual stack instead of va_args (host stack) */

#include "vstack.h"

static inline void* Barray(int n) {
  int i;
  data* r= (data*) alloc_array(n);

  for (i = n - 1; i >= 0; --i) {
    ((int*)r->contents)[i] = POP();
  }

  return r->contents;
}

static inline void* Bsexp(int n, int tag) {
  int fields_cnt = n;
  data* r = (data *)alloc_sexp(fields_cnt);
  ((sexp *)r)->tag = 0;

  for (int i = n; i > 0; --i) {
    ((int *)r->contents)[i] = POP();
  }

  ((sexp *)r)->tag = tag;

  return (int *)r->contents;
}

static inline void* Bclosure(int n, void* entry) {
  data* r = (data*) alloc_closure(n + 1);
  ((void **)r->contents)[0] = entry;

  for (int i = n; i >= 1; --i) {
    ((int *)r->contents)[i] = POP();
  }

  return r->contents;
}

#endif
