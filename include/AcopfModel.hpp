#pragma once

#include "PowerGrid.hpp"

#include <limits>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace acopf {

// Small adjacency record for Ybus row i -> column j.
struct YbusEdge {
  int to = -1;
  double G = 0.0;
  double B = 0.0;
};

// Branch physics in the unified branch model (line/tap/phase-shift).
struct BranchPhys {
  int id = -1;
  int f = -1;
  int t = -1;

  double Gff = 0.0, Bff = 0.0;
  double Gft = 0.0, Bft = 0.0;
  double Gtf = 0.0, Btf = 0.0;
  double Gtt = 0.0, Btt = 0.0;

  // If 0 => no thermal limit enforced (constraint skipped).
  double limit_sq = 0.0;
};

class AcopfModel {
public:
  explicit AcopfModel(const PowerGrid& grid, std::string output_dir);

  // Dimensions
  int nvar() const;
  int ncon() const;

  // Sparsity sizes
  long long nnz_jac() const;
  long long nnz_hess() const;

  // NLP interface used by solvers
  void bounds(double* x_l, double* x_u, double* g_l, double* g_u) const;
  void x0(double* x) const;

  bool eval_f(const double* x, double& obj_value) const;
  bool eval_grad_f(int n, const double* x, double* grad_f) const;

  bool eval_g(const double* x, double* g) const;

  long long jac_structure(int* iRow, int* jCol) const;
  long long jac_values(const double* x, double* values) const;

  long long hess_structure(int* iRow, int* jCol) const;
  bool eval_h(const double* x, double obj_factor, const double* lambda, double* values) const;

  void save_results(const double* x) const;

  // Stats filled by solver wrappers
  int last_iter_count = -1;
  double last_obj_value = std::numeric_limits<double>::quiet_NaN();

  // Handy for summary printing
  int nb = 0;
  int ng = 0;
  int nl = 0;

private:
  const PowerGrid& grid_;
  std::string output_dir_;

  // Index offsets in x
  int o_Vm_ = 0;
  int o_Va_ = 0;
  int o_Pg_ = 0;
  int o_Qg_ = 0;

  int ref_bus_ = -1;

  // Bus id -> internal index
  std::unordered_map<int, int> bus_id_to_idx_;

  // gens at each bus (internal generator indices)
  std::vector<std::vector<int>> bus_gens_;

  // adjacency for Ybus rows (merged)
  std::vector<std::vector<YbusEdge>> adj_;

  // diagonal Ybus (needed for Jacobian/Hessian shortcuts)
  std::vector<double> Gdiag_;
  std::vector<double> Bdiag_;

  // branch physics
  std::vector<BranchPhys> phys_br_;

  // Hessian structure: unique lower-tri pairs (r >= c)
  std::vector<std::pair<int, int>> hess_pos_;
  std::unordered_map<long long, int> hpos_map_;

private:
  static long long key_rc(int r, int c);
  static void sincos_fast(double x, double& s, double& c);

  void add_h(int r, int c, double v, double* values) const;

  void build_network();              // builds bus maps, adj_, phys_br_, diag
  void build_hessian_structure();    // builds hess_pos_ and hpos_map_

  void add_flow_hessian_dense(const BranchPhys& p,
                              double lam,
                              const double* x,
                              bool is_from,
                              double* values) const;
};

} // namespace acopf
