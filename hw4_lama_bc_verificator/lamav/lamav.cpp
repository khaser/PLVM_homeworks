// #define DEBUG
#include <vector>
#include <chrono>

#include "bytefile.h"
#include "sm_encoding.h"
#include "verify.h"
#include "interpret/interpret.h"


int main(int argc, char* argv[]) {
  bytefile* bf = read_file(argv[1]);

  std::vector<BcData> bc_data(bf->code_size + 1);
  std::vector<BcFunction> bc_funcs;

  std::chrono::steady_clock::time_point startup = std::chrono::steady_clock::now();

  for (auto offset : bf->public_offsets()) {
    int zero = 0;
    BcFunction fun {};
    dispatch<VerifyBytecode, void>(offset, bf, bc_data, bc_funcs, zero, fun, bf);
  }

  std::chrono::steady_clock::time_point verify_finish = std::chrono::steady_clock::now();

#ifdef DEBUG
  std::cout << "Reachable functions report:\n";
  std::cout << "Begin\tEnd\tMin st.\tMax st.\tArguments\n";
#endif
  for (auto fun : bc_funcs) {
#ifdef DEBUG
    std::cout << std::hex << fun.begin << '\t' << fun.end << '\t' << \
                 std::dec << fun.min_rel_st_size << '\t' << fun.max_rel_st_size << '\t' << \
                 fun.args << '\n';
#endif
    if (fun.min_rel_st_size < 0) {
      failure("Stack overflow detected in function %08x", fun.begin);
    }

    // Write extra stack space required to call this function
    *(short*)(bf->code_ptr + fun.begin + 2) = fun.max_rel_st_size - fun.args;
  }

  int ret = interpret(bf, argv[1]);
  std::chrono::steady_clock::time_point interpret_finish = std::chrono::steady_clock::now();

  auto verify_time = std::chrono::duration_cast<std::chrono::nanoseconds>(verify_finish - startup).count();
  auto interpret_time = std::chrono::duration_cast<std::chrono::nanoseconds>(interpret_finish - verify_finish).count();

  std::cout << "Validation time:\t" << verify_time << "[ns]\n";
  std::cout << "Interpretation time:\t" << interpret_time << "[ns]\n";
  return ret;
}
