# α-Spectrum Filter

## Theoretical Background:

* [Rényi entropy](https://en.wikipedia.org/wiki/R%C3%A9nyi_entropy)
* [Noncommutative geometry](https://en.wikipedia.org/wiki/Noncommutative_geometry)
* [Stochastic matrix](https://en.wikipedia.org/wiki/Stochastic_matrix)

## Discussion

This filter is conceptually modeled after mythical beasts like the hydra or the chimera: a creature with many heads, but a single coordinating body.

Each "head" represents a distinct theory about the nature of time and space. These are rigid mathematical assumptions:

* **Exponential Moving Average:** The workhorse filter, excellent on its own. Often used in audio filtering as an Infinite Impulse Response (IIR) filter.
* **Adaptive Threshold:** Gates in large movements and aggressively filters small ones for baseline noise rejection.
* **Brownian Motion:** A flat slider mapping kinetic energy in vs. kinetic energy out, providing strict thermodynamic grounding.
* **Predictive (The Falcon):** Assumes the next reading will follow the momentum of the last, computing a predicted boundary that is already in memory by the time the next frame arrives.

These filters are combined within an information-geometric universe, meaning the output of one filter may be mathematically "surprising" to another. This universe supports generalized dimensions, allowing us to use non-commutative geometry to represent raw optical camera projection.

In projective geometry, depth and scale are not orthogonal to the same degree that they are in standard 3D Cartesian coordinates. This orthogonality is broken by the property of non-commutativity. To solve this, we supply a commutator that gives us a "2.5D" gradient field. We then calculate the dot product of the incoming tracking data against this gradient. This isolates and rejects the logical incoherence that causes standard "Z-wobble" in tracking, establishing a physically "real" continuity as our ground truth.

To allow these wildly different tracking models to be compared, their signals are unified and parameterized using **Rényi entropy**. Because of this, their comparisons can be non-associative—meaning they can be composed in groups that are locally, logically incoherent.

Using a **Markov Transition Matrix (MTM)**, we resolve this non-associativity by supplying the theoretical minimum inductive bias for continuity. The MTM acts as a meta-filter (the "body" of the hydra), rejecting incoherent theories dynamically without prematurely throwing away useful edge-case information.

This architecture seamlessly resolves both non-commutative and non-associative inconsistencies between the many heads of the hydra. Practically, this means new kinematic models can be easily attached to the filter and toggled on and off during runtime without breaking the math, vastly simplifying future development.

To the best of our knowledge, this architecture represents the state-of-the-art in tracking filter technology, outperforming the Extended Kalman Filter (EKF) in almost every category for highly non-linear, low-latency human kinematics.

## Licensing

The generally available version is licensed under the **GNU GPLv3**.
Contact us for dual-licensing options for commercial or closed-source integration.
