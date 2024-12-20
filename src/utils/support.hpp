#pragma once

/// Allow macro generation of #pragma(arguments)
#define PRAGMA_WRAP(S) _Pragma(#S)

/// Enable application of macro over expanded var-args
#define IDENTITY(x)   x
#define APPLY(m, ...) IDENTITY(m(__VA_ARGS__))

/// If OpenMP is supported, build for-loop & SIMD parallel nestings
#if defined(BLASIUS_USE_OMP) && (__has_include(<omp.h>) && defined(_OPENMP))
#define BLASIUS_LOOP_PARALLEL(x) PRAGMA_WRAP(omp parallel for collapse(x))
#define BLASIUS_LOOP_PARALLEL_VA(x, ...) PRAGMA_WRAP(omp parallel for collapse(x) __VA_ARGS__)
#define BLASIUS_LOOP_SIMD PRAGMA_WRAP(omp simd)
#else
#define BLASIUS_LOOP_PARALLEL(x)
#define BLASIUS_LOOP_PARALLEL_VA(x, ...)
#define BLASIUS_LOOP_SIMD
#endif

#define BLASIUS_STRINGIFY_IMPL(x) #x
#define BLASIUS_STRINGIFY(VAR)    BLASIUS_STRINGIFY_IMPL(VAR)

// `assume` is not supported, define an empty macro
#if __has_cpp_attribute(202207L)
#define ASSUME(expr) [[assume(expr)]]
#else
#define ASSUME(expr)
#endif

// If `likely/unlikely` is not supported, define empty macros
// Otherwise, define IF/ELSE statements allowing use.
#if __cplusplus >= (202002L)
#define IF_LIKELY(expr)     if (expr) [[likely]]
#define IF_UNLIKELY(expr)   if (expr) [[unlikely]]
#define ELIF_LIKELY(expr)   else if (expr) [[likely]]
#define ELIF_UNLIKELY(expr) else if (expr) [[unlikely]]
#define ELSE_LIKELY         else [[likely]]
#define ELSE_UNLIKELY       else [[unlikely]]
#else
#define IF_LIKELY(expr)     if (expr)
#define IF_UNLIKELY(expr)   if (expr)
#define ELIF_LIKELY(expr)   else if (expr)
#define ELIF_UNLIKELY(expr) else if (expr)
#define ELSE_LIKELY         else
#define ELSE_UNLIKELY       else
#endif

#if defined(__GNUC__) || defined(__clang__)
#define BLASIUS_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define BLASIUS_UNREACHABLE __assume(false)
#else // Unsupported compiler
#define PL_UNREACHABLE
#endif
