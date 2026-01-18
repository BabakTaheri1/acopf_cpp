# Mathematical Formulation: AC Optimal Power Flow

This document details the exact **Non-Convex Non-Linear Programming (NLP)** problem implemented in the C++ `AcopfModel`. The formulation decouples the physical grid modeling from the numerical solver, using exact Hessian matrices for robust convergence.

It utilizes a **Unified Branch Model**, generalizing transmission lines, phase-shifting transformers, and tap-changing transformers into a single admittance structure.

## 1. Nomenclature & Sets

### 1.1 Indices and Sets
| Symbol | Definition |
| :--- | :--- |
| $\mathcal{N}$ | Set of buses (nodes), indexed by $i, j$. |
| $\mathcal{G}$ | Set of generators, indexed by $k$. |
| $\mathcal{L}$ | Set of branches (transmission lines & transformers), indexed by $l$. |
| $\mathcal{G}_i$ | Subset of generators connected to bus $i$. |

### 1.2 Decision Variables
The state vector $x \in \mathbb{R}^{2|\mathcal{N}| + 2|\mathcal{G}|}$ consists of:

* **Voltage State:**
  * $v_i$: Voltage magnitude at bus $i$ (p.u.).
  * $\theta_i$: Voltage angle at bus $i$ (radians).
* **Generation Dispatch:**
  * $p_{g,k}$: Active power output of generator $k$ (p.u.).
  * $q_{g,k}$: Reactive power output of generator $k$ (p.u.).

---

## 2. Network Physics: The Unified $\pi$-Model

To handle transmission lines and transformers simultaneously, every branch $l \in \mathcal{L}$ connecting bus $i$ (from) to bus $j$ (to) is modeled as an asymmetric $\pi$-equivalent circuit.

This model explicitly incorporates the **Complex Transformer Tap Ratio** $a_l$:

$$
a_l = \tau_l \cdot e^{j \sigma_l}
$$

* $\tau_l$: Off-nominal turns ratio ($\tau=1$ for standard lines).
* $\sigma_l$: Phase shift angle ($\sigma=0$ for standard lines).

### 2.1 Branch Admittance Parameters
Given the branch series impedance $z_s = r_l + jx_l$ and total charging susceptance $b_c$, we define the series admittance $y_s = 1/z_s$.

The four admittance parameters used in the Jacobian/Hessian calculations are derived as:

$$
\begin{aligned}
Y_{ff,l} &= \frac{y_s + j\frac{b_c}{2}}{|a_l|^2} & \quad \text{(From-side self admittance)} \\
Y_{ft,l} &= -\frac{y_s}{a_l^*} & \quad \text{(From-to mutual admittance)} \\
Y_{tf,l} &= -\frac{y_s}{a_l} & \quad \text{(To-from mutual admittance)} \\
Y_{tt,l} &= y_s + j\frac{b_c}{2} & \quad \text{(To-side self admittance)}
\end{aligned}
$$

> **Note:** If $\tau \neq 1$ or $\sigma \neq 0$, the resulting admittance matrix is asymmetric ($Y_{ft} \neq Y_{tf}$), which correctly models the physics of regulating transformers.

---

## 3. Optimization Problem

### 3.1 Objective Function
Minimize the total polynomial cost of active power generation:

$$
\min_{x} \quad J(x) = \sum_{k \in \mathcal{G}} \left( c_{2,k} (p_{g,k})^2 + c_{1,k} p_{g,k} + c_{0,k} \right)
$$

### 3.2 Constraints

#### Nodal Power Balance (Kirchhoff's Laws)
For every bus $i \in \mathcal{N}$:

$$
\begin{aligned}
\sum_{k \in \mathcal{G}_i} p_{g,k} - P_{d,i} &= v_i \sum_{j \in \mathcal{N}} v_j (G_{ij} \cos \theta_{ij} + B_{ij} \sin \theta_{ij}) \\
\sum_{k \in \mathcal{G}_i} q_{g,k} - Q_{d,i} &= v_i \sum_{j \in \mathcal{N}} v_j (G_{ij} \sin \theta_{ij} - B_{ij} \cos \theta_{ij})
\end{aligned}
$$

Where:
* $P_{d,i}, Q_{d,i}$: Active and reactive load demand.
* $G_{ij}, B_{ij}$: Real and imaginary parts of the nodal admittance matrix $Y_{bus}$.
* $\theta_{ij} = \theta_i - \theta_j$.

#### Branch Thermal Limits
We constrain the **squared apparent power** ($S^2$) to strictly avoid non-differentiable square roots in the solver derivatives.

For every branch $l$ (from $i \to j$):

$$
\begin{aligned}
|S_{ij}|^2 &= \left| v_i e^{j\theta_i} \cdot \left( Y_{ff,l} v_i e^{j\theta_i} + Y_{ft,l} v_j e^{j\theta_j} \right)^* \right|^2 \leq (RateA_l)^2 \\
|S_{ji}|^2 &= \left| v_j e^{j\theta_j} \cdot \left( Y_{tf,l} v_i e^{j\theta_i} + Y_{tt,l} v_j e^{j\theta_j} \right)^* \right|^2 \leq (RateA_l)^2
\end{aligned}
$$

#### Operational Limits
$$
\begin{aligned}
v^{min}_i \leq \quad &v_i \quad \leq v^{max}_i \\
p^{min}_k \leq \quad &p_{g,k} \quad \leq p^{max}_k \\
q^{min}_k \leq \quad &q_{g,k} \quad \leq q^{max}_k
\end{aligned}
$$

#### Reference Bus
To ensure a unique voltage angle solution:
$$
\theta_{ref} = 0
$$