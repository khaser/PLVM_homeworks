#ifndef VERIFY_H
#define VERIFY_H

#include "sm_encoding.h"
#include "bytefile.h"
#include "dispatch.h"

#include <unordered_map>

namespace {
/*
 * Return all theoretically possible next insn offsets and current insn bytecode enum
 */

inline bool check_reachable(int offset, int stack_sz, std::vector<BcData> &bc_data) {
  if (bc_data[offset].reachable) {
    if (bc_data[offset].rel_st_size != stack_sz) {
      std::cout << "Bytecode has broken stack invariant for offset: " << std::hex << offset << "!\n";
      exit(1);
    } else return true;
  }
  bc_data[offset].reachable = true;
  bc_data[offset].rel_st_size = stack_sz;
  return false;
}

#define DEBUG do { dispatch<PrintCode, void>(offset, bf);\
                   std::cout << ' ' << stack_sz << '\n'; } while (0);

template<Bytecodes B, class... Opnds>
struct VerifyBytecode {
  void operator () (int offset, int sz, const char* str,
                    std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs,
                    int stack_sz, BcFunction &cur_fun, bytefile* bf,
                    Opnds... opnds) const {

    if (check_reachable(offset, stack_sz, bc_data)) return;

    DEBUG

    cur_fun.min_rel_st_size = std::min(cur_fun.min_rel_st_size, stack_sz + STACK_SZ_OVERFLOW(B));
    cur_fun.max_rel_st_size = std::max(cur_fun.max_rel_st_size, stack_sz + STACK_SZ_DELTA(B));

    if (B == BC_STOP || B == BC_FAIL) return;
    if (B == BC_END) {
      cur_fun.end = offset;
      if (stack_sz - 1 != cur_fun.args) {
        std::cout << "Stack size - 1 != number of arguments on function exit at address: " << std::hex << cur_fun.end << '\n';
        std::cout << "Estimated stack size is: " << std::dec << stack_sz << '\n';
        std::cout << "Number of arguments : " << std::dec << cur_fun.args << '\n';
        exit(1);
      }
      return;
    }

    dispatch<VerifyBytecode, void>(offset + sz, bf, bc_data, bc_funcs, stack_sz + STACK_SZ_DELTA(B), cur_fun, bf);
  }
};

template<Bytecodes B>
requires (B == BC_BARRAY)
struct VerifyBytecode<B, int> {
  void operator () (int offset, int sz, const char* str,
                    std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs,
                    int stack_sz, BcFunction &cur_fun, bytefile* bf,
                    int array_sz) const {

    if (check_reachable(offset, stack_sz, bc_data)) return;

    DEBUG

    int new_sz = stack_sz - array_sz + 1;
    cur_fun.min_rel_st_size = std::min(cur_fun.min_rel_st_size, new_sz);
    cur_fun.max_rel_st_size = std::max(cur_fun.max_rel_st_size, new_sz);

    dispatch<VerifyBytecode, void>(offset + sz, bf, bc_data, bc_funcs, new_sz, cur_fun, bf);
  }
};

template<Bytecodes B>
requires (B == BC_SEXP)
struct VerifyBytecode<B, int, int> {
  void operator () (int offset, int sz, const char* str,
                    std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs,
                    int stack_sz, BcFunction &cur_fun, bytefile* bf,
                    int tag_id, int tag_sz) const {

    if (check_reachable(offset, stack_sz, bc_data)) return;

    DEBUG

    int new_sz = stack_sz - tag_sz + 1;
    cur_fun.min_rel_st_size = std::min(cur_fun.min_rel_st_size, new_sz);
    cur_fun.max_rel_st_size = std::max(cur_fun.max_rel_st_size, new_sz);

    dispatch<VerifyBytecode, void>(offset + sz, bf, bc_data, bc_funcs, new_sz, cur_fun, bf);
  }
};

template<Bytecodes B>
requires (B == BC_CJMPZ || B == BC_JMP || B == BC_CJMPNZ)
struct VerifyBytecode<B, int> {
  void operator () (int offset, int sz, const char* str,
                    std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs,
                    int stack_sz, BcFunction &cur_fun, bytefile* bf,
                    int label_offset) const {

    if (check_reachable(offset, stack_sz, bc_data)) return;

    cur_fun.min_rel_st_size = std::min(cur_fun.min_rel_st_size, stack_sz + STACK_SZ_OVERFLOW(B));
    cur_fun.max_rel_st_size = std::max(cur_fun.max_rel_st_size, stack_sz + STACK_SZ_DELTA(B));

    DEBUG

    if (B == BC_JMP) {
      dispatch<VerifyBytecode, void>(label_offset, bf, bc_data, bc_funcs, stack_sz + STACK_SZ_DELTA(B), cur_fun, bf);
    } else {
      dispatch<VerifyBytecode, void>(label_offset, bf, bc_data, bc_funcs, stack_sz + STACK_SZ_DELTA(B), cur_fun, bf);
      dispatch<VerifyBytecode, void>(offset + sz, bf, bc_data, bc_funcs, stack_sz + STACK_SZ_DELTA(B), cur_fun, bf);
    }
  }
};

template<Bytecodes B>
requires (B == BC_CALL || B == BC_CLOSURE)
struct VerifyBytecode<B, int, int> {
  void operator () (int offset, int sz, const char* str,
                    std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs,
                    int stack_sz, BcFunction &cur_fun, bytefile* bf,
                    int label_offset, int args) const {

    if (check_reachable(offset, stack_sz, bc_data)) return;

    cur_fun.min_rel_st_size = std::min(cur_fun.min_rel_st_size, stack_sz + STACK_SZ_OVERFLOW(B));
    cur_fun.max_rel_st_size = std::max(cur_fun.max_rel_st_size, stack_sz + STACK_SZ_DELTA(B));

    DEBUG

    dispatch<VerifyBytecode, void>(offset + sz, bf, bc_data, bc_funcs, stack_sz - args + 1, cur_fun, bf);

    if (B == BC_CALL && bf->code_ptr[label_offset] != BC_BEGIN) {
        std::cout << "Call on offset " << std::hex << offset << " does not refer to BEGIN bytecode" << '\n';
        exit(1);
    }
    // Standard compiler generates ill formatted code
    // if (B == BC_CLOSURE && bf->code_ptr[label_offset] != BC_CBEGIN) {
    //     std::cout << "Closure on offset " << std::hex << offset << " does not refer to CBEGIN bytecode" << '\n';
    //     exit(1);
    // }
    dispatch<VerifyBytecode, void>(label_offset, bf, bc_data, bc_funcs, 0, BcFunction {}, bf);
  }
};

template<Bytecodes B>
requires (B == BC_BEGIN || B == BC_CBEGIN)
struct VerifyBytecode<B, int, int> {
  void operator () (int offset, int sz, const char* str,
                    std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs,
                    int stack_sz, BcFunction &cur_fun, bytefile* bf,
                    int args, int _locals) const {

    if (check_reachable(offset, 0, bc_data)) return;

    DEBUG

    cur_fun.begin = offset;
    cur_fun.args = args;

    dispatch<VerifyBytecode, void>(offset + sz, bf, bc_data, bc_funcs, args, cur_fun, bf);
    bc_funcs.push_back(cur_fun);

  }
};

} // anon namespace

void verify_entrypoint(int offset, bytefile* bf,
                       std::vector<BcData> &bc_data, std::vector<BcFunction> &bc_funcs) {
  dispatch<VerifyBytecode, void>(offset, bf, bc_data, bc_funcs, 0, BcFunction {}, bf);
}

#endif
