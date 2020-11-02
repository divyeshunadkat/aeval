apara
=====

A new tool for constrained Horn clauses (CHC) based on the Expression library of <a href="http://seahorn.github.io/">SeaHorn</a> and the <a href="https://github.com/Z3Prover/z3">Z3</a> SMT solver.

Installation
============

Compiles with gcc-5 (on Linux) and clang-900 (on Mac). Assumes preinstalled Svn, Gmp, and Boost packages. Additionally, armadillo package to get candidates from behaviors.

* `cd aeval ; mkdir build ; cd build`
* `cmake ../`
* `make` to build dependencies (Z3 and LLVM)
* `make` (again) to build apara

The binary of apara can be found at `build/tools/apara/`.
Run `./tools/apara/apara --help` for the usage info.

Benchmarks
==========

Collection of benchmarks is underprogress.
