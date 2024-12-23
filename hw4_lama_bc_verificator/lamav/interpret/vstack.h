#ifndef VSTACK_H
#define VSTACK_H

#include <cstddef>
#define MAX_VSTACK_SZ      10000
#define MAX_FRAME_STACK_SZ 10000

extern size_t* __gc_stack_top; /* points to first uninitialized vstack element */
extern size_t* __gc_stack_bottom;

#define PUSH(val) (*(__gc_stack_top--) = val)
#define PUSH_REF(val) PUSH((size_t) (val))
#define POP() (*(++__gc_stack_top))
#define POP_REF() (size_t*) POP()
#define TOP() (*(__gc_stack_top+1))
#define ALLOC(n) do { __gc_stack_top -= (n); \
                      memset(__gc_stack_top + 1, 0, n * sizeof(size_t)); \
                 } while (0)
#define TRUNC(n) (__gc_stack_top += (n))
#define STACK_SZ (__gc_stack_bottom - __gc_stack_top - 1)
#define STACK_FREE_SPACE (MAX_VSTACK_SZ - STACK_SZ)

#endif
