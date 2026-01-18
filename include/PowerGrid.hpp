#pragma once

#include <vector>

namespace acopf {

// ---------------- Bus ----------------
struct Bus {
  int id;        // MATPOWER BUS_I
  int type;      // 1=PQ, 2=PV, 3=Slack
  double Pd, Qd; // load (MW, Mvar)
  double Gs, Bs; // shunt (MW, Mvar at V=1)
  double Vm, Va; // initial voltage (p.u., degrees)
  double Vmax, Vmin;
};

// ---------------- Generator ----------------
struct Gen {
  int bus_id;
  double Pg, Qg;
  double Qmax, Qmin;
  double status;
  double Pmax, Pmin;
};

// ---------------- Branch ----------------
struct Branch {
  int f_bus, t_bus;
  double r, x, b;
  double rateA;
  double tap, shift;
  double status;
};

// ---------------- Generator cost ----------------
struct GenCost {
  double c2, c1, c0;   // quadratic cost
};

// ---------------- Power grid container ----------------
struct PowerGrid {
  double baseMVA = 100.0;

  std::vector<Bus> buses;
  std::vector<Gen> gens;
  std::vector<Branch> branches;
  std::vector<GenCost> costs;
};

} // namespace acopf
