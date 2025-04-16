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
