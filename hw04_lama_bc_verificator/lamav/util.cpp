#include "util.h"
#include <vector>

std::ostream& operator << (std::ostream &stream, const BcLoc &loc) {
  return stream << "(" << (int) loc.type << ", " << loc.idx << ")";
}

std::ostream& operator << (std::ostream &stream, const std::vector<BcLoc> &locs) {
  stream << "{";
  for (auto &loc : locs) {
    stream << loc << ", ";
  }
  return stream << "}";
}

