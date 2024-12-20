#include <memory>
#include <vector>
#include <span>
#include <functional>
#include <utility>

#include "bytefile.h"
#include "sm_encoding.h"
#include "verify.h"
#include "take_bytecode.h"

int main(int argc, char* argv[]) {
  bytefile* bf = read_file(argv[1]);

  std::vector<BcData> bc_data(bf->code_size + 1);
  std::vector<BcFunction> bc_funcs;

  for (auto offset : bf->public_offsets()) {
    int zero = 0;
    BcFunction fun {};
    dispatch<VerifyBytecode, void>(offset, bf, bc_data, bc_funcs, zero, fun, bf);
  }

  std::cout << "Reachable functions report:\n";
  std::cout << "Begin\tEnd\tMin st.\tMax st.\tArguments\n";
  for (auto fun : bc_funcs) {
    std::cout << std::hex << fun.begin << '\t' << fun.end << '\t' << \
                 std::dec << fun.min_rel_st_size << '\t' << fun.max_rel_st_size << '\t' << \
                 fun.args << '\n';
    if (fun.min_rel_st_size < 0) {
      std::cout << "Stack overflow detected in function " << fun.begin << '\n';
      exit(1);
    }

    // Write extra stack space required to call this function
    *(short*)(bf->code_ptr + fun.begin + 2) = fun.max_rel_st_size - fun.args;
  }

  // Interpretation
  // TODO: take from lamai

  return 0;
}
