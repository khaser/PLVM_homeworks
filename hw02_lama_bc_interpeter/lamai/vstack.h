#ifndef VSTACK_H
#define VSTACK_H

#define MAX_VSTACK_SZ      10000
#define MAX_FRAME_STACK_SZ 10000

extern size_t* __gc_stack_top; /* points to first uninitialized vstack element */
extern size_t* __gc_stack_bottom;

#define CHECK_UNDERFLOW() (__gc_stack_bottom - __gc_stack_top < 0 && (failure("vstack underflow\n"), 42))
#define CHECK_OVERFLOW() (__gc_stack_bottom - __gc_stack_top > MAX_VSTACK_SZ && (failure("vstack overflow\n"), 228))

#define PUSH(val) (CHECK_OVERFLOW(), *(__gc_stack_top--) = val)
#define PUSH_REF(val) PUSH((int) (val))
#define POP() (__gc_stack_top++, CHECK_UNDERFLOW(), *__gc_stack_top)
#define POP_REF() (int*) POP()
#define TOP() (*(__gc_stack_top+1))
#define ALLOC(n) do { __gc_stack_top -= (n); CHECK_OVERFLOW(); \
                      memset(__gc_stack_top + 1, 0, n * sizeof(size_t)); \
                 } while (0)
#define TRUNC(n) do { __gc_stack_top += (n); CHECK_UNDERFLOW(); } while (0)

#endif
