#pragma once
#include <chrono>

namespace acopf {

class Timer {
public:
  Timer() : t0_(std::chrono::steady_clock::now()) {}
  double seconds_since_start() const;

  static double seconds_between(std::chrono::steady_clock::time_point a,
                                std::chrono::steady_clock::time_point b);

private:
  std::chrono::steady_clock::time_point t0_;
};

} // namespace acopf
