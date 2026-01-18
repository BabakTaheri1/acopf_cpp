#pragma once
#ifndef USE_KNITRO

#include "AcopfModel.hpp"
#include "SolveResult.hpp"

namespace acopf {
SolveResult SolveWithIpopt(AcopfModel& model);
} // namespace acopf

#endif
