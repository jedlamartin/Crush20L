# FLOPS Benchmark Branch

This branch is used for **computational performance analysis** of the core algorithms in the Crush20L project. It is not intended for production or merging into the main application. Here we use [PAPI (Performance API)](https://icl.utk.edu/papi/) to count floating-point operations and evaluate computational demand.

---

## Purpose

Measure the **FLOPs (floating point operations)** performed during audio processing stages to:

- Evaluate computational intensity of various stages
- Compare performance between algorithms or optimizations
- Guide hardware targeting decisions

---

## Dependencies

- **PAPI** (installed via package manager or built from source)
- **CMake** (version 3.10+)
- **C++17** compatible compiler

### On Linux:

```bash
sudo pacman -S papi
