# BLASius

Dynamic BLAS/LAPACK dispatch library through the C interface.

## Problem:

* We require a fixed BLAS/LAPACK library to support Lightning and Catalyst's internal functions with access via the C interface layer
* Choice of a fixed compile-time BLAS library sets the library version for our users
* We may wish to dynamically override the above library for better performance on a given mahcine (e.g using oneMKL, Apple Accelerate, AMD AOCL, etc)
* Symbols from easily-accessible BLAS/LAPACK libraries may not match the expected call-signatures (e.g. scipy-openblas32 prepends `scipy_` to all symbols)

## Solution

* Provide a base-line BLAS/LAPACK library API (e.g. scipy-openblas32) with dynamically overridable symbol dispatch
* Allow `LD_PRELOAD`/`DYLD_PRELOAD`ing to arbitrary supported BLAS/LAPACK implementations with changeable prefix/postfix strings
* Support implementation agnostic control over threading layers (where capable)
    * For pthreads, OpenMP, etc threading layers, allow a single end-point for thread control
* Ensure weak-symbol definitions for all API calls, such that each implementation can easily override 


## Technical considerations

As we intend to provide access through the C-interface layer only (to ensure the best compatibility across platforms, languages, etc), we do not intend to rely on the FORTRAN level interfaces to either BLAS or LAPACK, and instead will require access through `cblas.h` and `lapacke.h` provided function signatures. As such, it is worth noting the variation between the FORTRAN interface (column major data interface) and C interface (row major). Additional considerations for LAPACK can be found at https://www.netlib.org/lapack/lapacke.html

Most notably, the variants offer a two level approach for memory management: either all allocations happen within the API (high-level interface) or the user is required to handle all memory management required for calls to the respective functions (middle-level interface). Differentiation between these layers in LAPACK is provided by the `LAPACKE_<lower-case-func-name>` for the high-level interface, and `LAPACKE_<lower-case-func-name>_work` for the middle-level interface.