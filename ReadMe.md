# AC Optimal Power Flow (ACOPF) — C++ (Exact Jacobian + Exact Hessian) with Ipopt / Knitro

This is self-contained ACOPF solver written in C++. It loads a MATPOWER `.mat` case file, builds the full nonlinear ACOPF with **exact** first- and second-order derivatives, and writes the solution out as simple CSV files.

---

## What it does

Given a MATPOWER case:

- Variables
  - Bus voltage magnitudes `Vm`
  - Bus voltage angles `Va` (radians internally)
  - Generator active power `Pg`
  - Generator reactive power `Qg`

- Constraints
  - AC power balance (P/Q) at each bus
  - Branch flow limits (implemented as a current-magnitude-squared style constraint for both directions)
  - Slack/reference bus angle fixed to 0

- Objective
  - Standard polynomial generation cost (quadratic if available; otherwise linear/constant)

- Output
  - `results/<case_name>/solution_bus.csv`
  - `results/<case_name>/solution_gen.csv`
  - Runtime breakdown (load / init / solve / total)

---

## Dependencies

You’ll need:

- **Ipopt** (headers + libraries)
- **Matio** (to read MATPOWER `.mat` files)
- A BLAS/LAPACK stack (usually pulled in via Ipopt)
- A linear solver for Ipopt  
  - Default: **MUMPS** (`linear_solver = mumps`)
  - If unavailable, switch to another installed option (e.g., MA27/MA57, Pardiso, etc.)

**Optional**
- **Knitro** (requires a separate Knitro installation and license; not distributed with this repo)

---

## Build

Default solver is **Ipopt**:

```bash
make
```
```bash
make knitro
```
  
Then run
```
./acopf test_cases/pglib_opf_case30000_gocc.mat
```

```
./acopf_knitro test_cases/pglib_opf_case30000_gocc.mat
```

## Benchmarks — Ipopt vs Knitro 

| Case | Obj (Ipopt) | Obj (Knitro) | Δ Obj (K − I) | Solve(s) Ipopt | Solve(s) Knitro | Speedup (I/K) | Total(s) Ipopt | Total(s) Knitro | Iter Ipopt | Iter Knitro |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `pglib_opf_case118_ieee` | 9.721361e+04 | 9.721361e+04 | -3.41e-03 | 0.023 | 0.009 | **2.56×** | 0.025 | 0.009 | 18 | 9 |
| `pglib_opf_case300_ieee` | 5.652200e+05 | 5.652200e+05 | -9.11e-03 | 0.046 | 0.025 | **1.84×** | 0.048 | 0.027 | 21 | 12 |
| `pglib_opf_case1354_ieee` | 1.258844e+06 | 1.258844e+06 | -3.72e-03 | 0.280 | 0.148 | **1.89×** | 0.290 | 0.156 | 34 | 20 |
| `pglib_opf_case2000_ieee` | 1.228892e+06 | 1.228892e+06 | +7.55e-02 | 0.560 | 0.264 | **2.12×** | 0.610 | 0.309 | 30 | 16 |
| `pglib_opf_case2848_rte` | 1.286608e+06 | 1.286608e+06 | +1.85e-02 | 3.785 | 1.360 | **2.78×** | 3.800 | 1.374 | 163 | 69 |
| `pglib_opf_case2868_rte` | - | 2.009605e+06 | — | — | 1.311 | — | — | 1.326 | — | 70 |
| `pglib_opf_case2869_pegase` | 2.462790e+06 | 2.462790e+06 | +2.04e-02 | 0.818 | 0.298 | **2.74×** | 0.834 | 0.315 | 43 | 23 |
| `pglib_opf_case3022_goc` | 6.013838e+05 | 6.013838e+05 | +4.71e-02 | 0.741 | 0.260 | **2.85×** | 0.760 | 0.275 | 43 | 21 |
| `pglib_opf_case3120sp_k` | 2.147969e+06 | 2.147969e+06 | +1.07e-01 | 0.779 | 0.246 | **3.17×** | 0.796 | 0.260 | 49 | 20 |
| `pglib_opf_case4020_goc` | 8.222473e+05 | 8.222473e+05 | +1.31e-02 | 1.891 | 0.729 | **2.59×** | 1.917 | 0.752 | 37 | 25 |
| `pglib_opf_case4661_sdet` | 2.251344e+06 | 2.251344e+06 | +8.38e-02 | 1.741 | 0.474 | **3.67×** | 1.765 | 0.497 | 44 | 22 |
| `pglib_opf_case5658_epigrids` | 1.207314e+06 | 1.207314e+06 | -2.08e-01 | 1.723 | 0.526 | **3.28×** | 1.754 | 0.557 | 37 | 17 |
| `pglib_opf_case6468_rte` | — | 2.069730e+06 | — | — | 6.575 | — | — | 6.607 | — | 196 |
| `pglib_opf_case6495_rte` | — | 3.067826e+06 | — | — | 3.929 | — | — | 3.962 | — | 95 |
| `pglib_opf_case6515_rte` | — | 2.825500e+06 | — | — | 4.694 | — | — | 4.726 | — | 126 |
| `pglib_opf_case7336_epigrids` | 1.882390e+06 | 1.882390e+06 | +2.38e-02 | 2.167 | 0.787 | **2.75×** | 2.205 | 0.823 | 38 | 20 |
| `pglib_opf_case9241_pegase` | 6.243090e+06 | **6.242484e+06** | -6.06e+02 | 10.831 | 7.823 | **1.38×** | 10.883 | 7.873 | 141 | 55 |
| `pglib_opf_case9591_goc` | 1.061684e+06 | 1.061684e+06 | -4.27e-01 | 3.898 | 1.521 | **2.56×** | 3.944 | 1.564 | 42 | 22 |
| `pglib_opf_case10000_goc` | 1.354031e+06 | 1.354031e+06 | +3.46e-01 | 11.208 | 6.615 | **1.69×** | 11.267 | 6.660 | 165 | 83 |
| `pglib_opf_case10192_epigrids` | 1.686923e+06 | 1.686923e+06 | +1.46e-02 | 4.700 | 1.351 | **3.48×** | 4.748 | 1.403 | 53 | 20 |
| `pglib_opf_case10480_goc` | 2.314648e+06 | 2.314648e+06 | +2.35e-02 | 5.965 | 1.819 | **3.28×** | 6.022 | 1.870 | 50 | 24 |
| `pglib_opf_case13659_pegase` | 8.948049e+06 | **8.948048e+06** | -1.29e+00 | 7.373 | 5.734 | **1.29×** | 7.443 | 5.799 | 71 | 48 |
| `pglib_opf_case19402_goc` | 1.977354e+06 | 1.977354e+06 | +2.40e-02 | 10.464 | 4.095 | **2.56×** | 10.558 | 4.188 | 48 | 27 |
| `pglib_opf_case20758_epigrids` | 2.617867e+06 | 2.617867e+06 | +7.47e-03 | 11.886 | 2.606 | **4.56×** | 11.981 | 2.699 | 72 | 20 |
| `pglib_opf_case24464_goc` | 2.629531e+06 | 2.629531e+06 | -1.29e-01 | 12.228 | 5.633 | **2.17×** | 12.336 | 5.743 | 63 | 34 |
| `pglib_opf_case30000_goc` | 1.142332e+06 | 1.142332e+06 | -3.89e-01 | 21.827 | 30.646 | **0.71×** | 21.940 | 30.757 | 131 | 192 |
| `pglib_opf_case30000_goc__api` | 1.776875e+06 | 1.776875e+06 | +1.04e-01 | 91.831 | 29.108 | **3.15×** | 91.939 | 29.215 | 405 | 176 |
| `pglib_opf_case30000_goc__sad` | 1.142332e+06 | 1.142332e+06 | -7.16e-02 | 22.875 | 30.652 | **0.75×** | 22.987 | 30.767 | 131 | — |

**Notes**
- Speedup = Solve time (Ipopt) / Solve time (Knitro)
- Speedup > 1 ⇒ Knitro is faster

**Experimental Setup**
- Platform: Apple MacBook (Apple M4 CPU, 16 GB RAM, macOS)