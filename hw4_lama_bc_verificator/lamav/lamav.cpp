#define _GLIBCXX_DEBUG
#include <memory>
#include <vector>
#include <span>
#include <functional>
#include <optional>

#include "bytefile.h"
#include "sm_encoding.h"
#include "verify.h"
#include "take_bytecode.h"


int main(int argc, char* argv[]) {
  bytefile* bf = read_file(argv[1]);

  std::vector<BcData> bc_data(bf->code_size + 1);
  std::vector<BcFunction> bc_funcs;

  // // Verification traverse
  // {
  //   std::function<void(int, int, BcFunction)> verify_traverse = [&] (int offset, int rel_st_size, BcFunction cur_fun) {
  //     bc_data[offset].reachable = true;
  //     bc_data[offset].rel_st_size = rel_st_size;
  //
  //     dispatch<PrintCode, void>(offset, bf);
  //     std::cout << '\n';
  //
  //     auto [adj_offsets, cur_bc] = dispatch<AllConts, std::pair<std::vector<int>, enum Bytecodes>>(offset, bf);
  //     // std::cerr << rel_st_size << ' ' << BC_BINOP << ' ' << BC_BEGIN << ' ' << cur_bc << ' ' << '\n';
  //
  //     int after_stack_sz = rel_st_size + STACK_SZ_DELTA(cur_bc);
  //     cur_fun.min_rel_st_size = std::min(rel_st_size + STACK_SZ_OVERFLOW(cur_bc), cur_fun.min_rel_st_size);
  //     if (cur_bc == BC_BEGIN || cur_bc == BC_CBEGIN) {
  //       after_stack_sz = 0;
  //       cur_fun = {offset, -1, 0};
  //     }
  //     if (cur_bc == BC_END) {
  //       cur_fun.end = offset;
  //       bc_funs.push_back(cur_fun);
  //     }
  //
  //     for (int to_offset : adj_offsets) {
  //       if (bc_data[to_offset].reachable) {
  //         if (after_stack_sz != bc_data[to_offset].rel_st_size) {
  //           std::cout << "Bytecode has broken stack invariant for offset: " << std::hex << to_offset << "!\n";
  //           exit(1);
  //         }
  //       } else {
  //         verify_traverse(to_offset, after_stack_sz, cur_fun);
  //       }
  //     }
  //   };
  //
  //   for (auto offset : bf->public_offsets()) {
  //     if (!bc_data[offset].reachable) {
  //       verify_traverse(offset, 0, {offset, -1, 0});
  //     }
  //   }
  // }

  for (auto offset : bf->public_offsets()) {
    verify_entrypoint(offset, bf, bc_data, bc_funcs);
  }

  for (auto fun : bc_funcs) {
    std::cout << std::hex << fun.begin << ' ' << fun.end << ' ' << std::dec << fun.min_rel_st_size << '\n';
  }

  // Interpretation
  // TODO: take from lamai

  return 0;
}
