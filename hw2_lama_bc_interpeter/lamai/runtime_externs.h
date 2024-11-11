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

#endif
