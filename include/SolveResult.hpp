#pragma once
#include <limits>
#include <string>

namespace acopf {

struct SolveResult {
  std::string solver;
  std::string status_str;
  double obj = std::numeric_limits<double>::quiet_NaN();
  int iters = -1;
};

} // namespace acopf
