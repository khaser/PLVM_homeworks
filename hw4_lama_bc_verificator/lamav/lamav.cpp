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

  for (auto offset : bf->public_offsets()) {
    dispatch<VerifyBytecode, void>(offset, bf, bc_data, bc_funcs, 0, BcFunction {}, bf);
  }

  for (auto fun : bc_funcs) {
    std::cout << std::hex << fun.begin << ' ' << fun.end << ' ' << std::dec << fun.min_rel_st_size << '\n';
  }

  // Interpretation
  // TODO: take from lamai

  return 0;
}
