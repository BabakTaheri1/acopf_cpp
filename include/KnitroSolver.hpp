#pragma once
#ifdef USE_KNITRO

#include "AcopfModel.hpp"
#include "SolveResult.hpp"

namespace acopf {
SolveResult SolveWithKnitro(AcopfModel& model);
} // namespace acopf

#endif
