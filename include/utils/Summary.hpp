#pragma once
#include <string>

namespace acopf {

void AppendSummaryCsv(const std::string& summary_csv,
                      const std::string& case_name,
                      const std::string& solver_name,
                      const std::string& status_str,
                      double obj,
                      int iters,
                      int nb, int ng, int nl,
                      double data_s,
                      double init_s,
                      double solve_s,
                      double total_s);

void PrintSummaryTable(const std::string& case_name,
                       const std::string& solver_name,
                       const std::string& status_str,
                       double obj,
                       int iters,
                       int nb, int ng, int nl,
                       double data_s,
                       double init_s,
                       double solve_s,
                       double total_s);

} // namespace acopf
