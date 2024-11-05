#ifndef RUNTIME_EXTERNS_H
#define RUNTIME_EXTERNS_H

extern int Lread ();
extern int Lwrite (int n);
extern int Llength (void *p);
extern void *Lstring (void *p);

#endif
