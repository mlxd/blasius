# BLASius macOS arm64 smoke test (throwaway)

Standalone probes that check macOS assumptions behind BLASius's provider design. They
run on GitHub's `macos-15` and `macos-26` runners and use no BLASius code. This
branch is never merged.

| Directory | Checks |
|---|---|
| `accel/` | Accelerate: `dlsym` of `$NEWLAPACK` / `$NEWLAPACK$ILP64` names, absence of LAPACKE, `BLASSetThreading` reach per symbol family, `cblas_i?amax` return width |
| `weak/` | Whether `DYLD_INSERT_LIBRARIES` of a provider overrides a weak stub (strong-only, plus a dummy weak def, `__interpose`) |
| `python/` | Which Python builds keep `DYLD_*`, their signing and dlopen flags; a `ctypes` preload satisfying `@rpath/libblasius` for an extension with no rpath |
| `rowmajor/` | Row-major in-place-transpose parity against netlib LAPACKE, both run on Accelerate's `$NEWLAPACK` LAPACK |
| `isolation/` | Two-level namespaces keeping an `RTLD_LOCAL` provider's own calls inside it, against weak/strong stubs and real OpenBLAS copies in global scope |
| `providers/` | Homebrew OpenBLAS keg path; ArmPL `_mp` by absolute path and its `libomp` |
| `baseline/` | Static OpenBLAS per width, renamed and localized without GNU `objcopy`, both widths linked into one binary |
