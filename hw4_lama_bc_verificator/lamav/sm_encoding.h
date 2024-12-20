#ifndef SM_ENCODING_H
#define SM_ENCODING_H

enum LocTypes {
    GLOBAL,
    LOCAL,
    ARGUMENT,
    CAPTURED,
};

enum Bytecodes {
  BC_BINOP    = 0,
  BC_LD       = 2,
  BC_LDA      = 3,
  BC_ST       = 4,
  BC_PATT     = 6,
  BC_STOP     = 15,

  /* 00-15 is BC_BINOP */

  BC_CONST    = 16,
  BC_STRING   = 17,
  BC_SEXP     = 18,
  BC_STI      = 19,
  BC_STA      = 20,
  BC_JMP      = 21,
  BC_END      = 22,
  BC_RET      = 23,
  BC_DROP     = 24,
  BC_DUP      = 25,
  BC_SWAP     = 26,
  BC_ELEM     = 27,

  /* 32-47 is BC_LD */
  /* 48-63 is BC_LDA */
  /* 64-79 is BC_ST */

  BC_CJMPZ    = 80,
  BC_CJMPNZ   = 81,
  BC_BEGIN    = 82,
  BC_CBEGIN   = 83,
  BC_CLOSURE  = 84,
  BC_CALLC    = 85,
  BC_CALL     = 86,
  BC_TAG      = 87,
  BC_ARRAY    = 88,
  BC_FAIL     = 89,
  BC_LINE     = 90,

  BC_PATT_STR = 96,
  /* 97-111 is BC_PATT for structures tags */

  BC_LREAD    = 112,
  BC_LWRITE   = 113,
  BC_LLENGTH  = 114,
  BC_LSTRING  = 115,
  BC_BARRAY   = 116
};

#define STACK_SZ_DELTA_(opcode, exp_opcode, delta) (exp_opcode == (opcode) ? delta : 0)

#define STACK_SZ_DELTA(opcode) (\
  STACK_SZ_DELTA_(opcode, BC_CONST, 1) | \
  STACK_SZ_DELTA_(opcode, BC_STRING, 1) | \
  STACK_SZ_DELTA_(opcode, BC_SEXP, 1) | \
  STACK_SZ_DELTA_(opcode, BC_STI, -1) | \
  STACK_SZ_DELTA_(opcode, BC_STA, -2) | \
  STACK_SZ_DELTA_(opcode, BC_JMP, 0) | \
  STACK_SZ_DELTA_(opcode, BC_END, 0) | \
  STACK_SZ_DELTA_(opcode, BC_DROP, -1) | \
  STACK_SZ_DELTA_(opcode, BC_DUP, 1) | \
  STACK_SZ_DELTA_(opcode, BC_SWAP, 0) | \
  STACK_SZ_DELTA_(opcode, BC_ELEM, -1) | \
  STACK_SZ_DELTA_(opcode, BC_CJMPZ, -1) | \
  STACK_SZ_DELTA_(opcode, BC_CJMPNZ, -1) | \
  STACK_SZ_DELTA_(opcode, BC_BEGIN, 0) | \
  STACK_SZ_DELTA_(opcode, BC_CBEGIN, 0) | \
  STACK_SZ_DELTA_(opcode, BC_CLOSURE, 1) | \
  STACK_SZ_DELTA_(opcode, BC_CALLC, 0) | \
  STACK_SZ_DELTA_(opcode, BC_CALL, 0) | \
  STACK_SZ_DELTA_(opcode, BC_TAG, 0) | \
  STACK_SZ_DELTA_(opcode, BC_ARRAY, 0) | \
  STACK_SZ_DELTA_(opcode, BC_FAIL, -1) | \
  STACK_SZ_DELTA_(opcode, BC_LINE, 0) | \
  STACK_SZ_DELTA_(opcode, BC_LREAD, 1) | \
  STACK_SZ_DELTA_(opcode, BC_LWRITE, 0) | \
  STACK_SZ_DELTA_(opcode, BC_LLENGTH, 0) | \
  STACK_SZ_DELTA_(opcode, BC_LSTRING, 0) | \
  STACK_SZ_DELTA_(opcode, BC_BARRAY, 1) | \
  STACK_SZ_DELTA_(opcode, BC_PATT_STR, -1) | \
  STACK_SZ_DELTA_(opcode, BC_BINOP, -1) | \
  STACK_SZ_DELTA_(opcode, BC_LD, 1) | \
  STACK_SZ_DELTA_(opcode, BC_LDA, 2) | \
  STACK_SZ_DELTA_(opcode, BC_ST, 0) | \
  STACK_SZ_DELTA_(opcode, BC_PATT, 0)\
)

/* All instructions do all pops from stack before all pushes, so
 * we can reuse STACK_SZ_DELTA to check underflows
 */

#define STACK_SZ_OVERFLOW_(opcode, exp_opcode, delta) (exp_opcode == (opcode) ? delta : 0)
#define STACK_SZ_OVERFLOW(opcode) (\
  STACK_SZ_OVERFLOW_(opcode, BC_CONST, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_STRING, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_SEXP, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_STI, -2) | \
  STACK_SZ_OVERFLOW_(opcode, BC_STA, -3) | \
  STACK_SZ_OVERFLOW_(opcode, BC_JMP, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_END, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_DROP, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_DUP, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_SWAP, -2) | \
  STACK_SZ_OVERFLOW_(opcode, BC_ELEM, -2) | \
  STACK_SZ_OVERFLOW_(opcode, BC_CJMPZ, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_CJMPNZ, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_BEGIN, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_CBEGIN, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_CLOSURE, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_CALLC, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_CALL, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_TAG, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_ARRAY, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_FAIL, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LINE, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LREAD, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LWRITE, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LLENGTH, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LSTRING, -1) | \
  STACK_SZ_OVERFLOW_(opcode, BC_BARRAY, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_PATT_STR, -2) | \
  STACK_SZ_OVERFLOW_(opcode, BC_BINOP, -2) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LD, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_LDA, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_ST, 0) | \
  STACK_SZ_OVERFLOW_(opcode, BC_PATT, -1)\
)

#define BINOPS(DEF) \
  DEF( 0, +)\
  DEF( 1, -)\
  DEF( 2, *)\
  DEF( 3, /)\
  DEF( 4, %)\
  DEF( 5, <)\
  DEF( 6, <=)\
  DEF( 7, >)\
  DEF( 8, >=)\
  DEF( 9, ==)\
  DEF(10, !=)\
  DEF(11, &&)\
  DEF(12, ||)

#endif
