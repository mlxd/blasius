# BLASius

Dynamic BLAS dispatch library.

## Problem:

* We require a fixed BLAS/LAPACK library to support Lightning and Catalyst's internal functions
* Choice of a fixed compile-time BLAS library sets the library version for our users
* We may wish to dynamically override the above library for better performance on a given mahcine (e.g using oneMKL, Apple Accelerate, AMD AOCL, etc)
* Symbols from easily-accessible BLAS/LAPACK libraries may not match the expected call-signatures (e.g. scipy-openblas32 prepends `scipy_` to all symbols)

## Solution

* Provide a base-line BLAS/LAPACK library API (e.g. scipy-openblas32) with dynamically overridable symbol dispatch
* Allow `LD_PRELOAD`/`DYLD_PRELOAD`ing to arbitrary supported BLAS/LAPACK implementations with changeable prefix/postfix strings
* Support implementation agnostic control over threading layers (where capable)
    * For pthreads, OpenMP, etc threading layers, allow a single end-point for thread control
* Ensure weak-symbol definitions for all API calls, such that each implementation can easily override 