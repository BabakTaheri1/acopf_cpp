#include "MatpowerLoader.hpp"
#include "AcopfModel.hpp"
#include "Solve.hpp"
#include "utils/Summary.hpp"
#include "utils/Timer.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

static void print_usage(const char* prog)
{
  std::cerr << "Usage: " << prog << " case.mat\n";
}

int main(int argc, char** argv)
{
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  const fs::path case_path(argv[1]);
  if (!fs::exists(case_path)) {
    std::cerr << "❌ File not found: " << case_path << "\n";
    return 1;
  }

  const std::string case_name  = case_path.stem().string();
  const fs::path    output_dir = fs::path("results") / case_name;

  std::error_code ec;
  fs::create_directories(output_dir, ec);
  if (ec) {
    std::cerr << "❌ Could not create output directory: " << output_dir
              << " (" << ec.message() << ")\n";
    return 1;
  }

  try {
    const auto t0 = std::chrono::steady_clock::now();

    acopf::PowerGrid grid = acopf::LoadMatpowerCase(case_path.string());
    const auto t_data = std::chrono::steady_clock::now();

    acopf::AcopfModel model(grid, output_dir.string());
    const auto t_init = std::chrono::steady_clock::now();

    acopf::SolveResult R = acopf::Solve(model);
    const auto t_solve = std::chrono::steady_clock::now();

    const double data_s  = acopf::Timer::seconds_between(t0, t_data);
    const double init_s  = acopf::Timer::seconds_between(t_data, t_init);
    const double solve_s = acopf::Timer::seconds_between(t_init, t_solve);
    const double total_s = acopf::Timer::seconds_between(t0, t_solve);

    acopf::PrintSummaryTable(case_name, R.solver, R.status_str, R.obj, R.iters,
                             model.nb, model.ng, model.nl,
                             data_s, init_s, solve_s, total_s);

    acopf::AppendSummaryCsv("results/summary.csv",
                            case_name, R.solver, R.status_str,
                            R.obj, R.iters,
                            model.nb, model.ng, model.nl,
                            data_s, init_s, solve_s, total_s);

    std::cout << "🧾 Appended run to results/summary.csv\n";
    std::cout << "✅ Finished. Solver=" << R.solver << " Status=" << R.status_str << "\n";
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "❌ Fatal error: " << e.what() << "\n";
    return 2;
  }
}
