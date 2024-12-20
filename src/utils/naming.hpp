/// Preprcoessor macro definitions for compile-time dynamic naming adjustments and utilities
#pragma once

#define EMPTY
#define NAME(x)                                                  x
#define NAME_DELIM(l, r, d)                                      l##d##r
#define FNAME_PRE(prefix, func_name)                             DELIM(prefix, func_name, _)
#define FNAME_POST(postfix, func_name)                           DELIM(func_name, postfix, _)

#define COUNT_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define COUNT(...)                                               COUNT_N(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define IDENTITY(x)                                              x
#define APPLY(m, ...)                                            IDENTITY(m(__VA_ARGS__))
#define CONCAT_STRINGIFY_DISPATCH(N)                             CAT_STR##N

#define STR(a1)                                                  #a1
#define CAT_STR1(a1)                                             STR(a1)
#define CAT_STR2(a1, a2)                                         STR(a1##a2)
#define CAT_STR3(a1, a2, a3)                                     STR(a1##a2##a3)
#define CAT_STR4(a1, a2, a3, a4)                                 STR(a1##a2##a3##a4)
#define CAT_STR5(a1, a2, a3, a4, a5)                             STR(a1##a2##a3##a4##a5)
#define CAT_STR6(a1, a2, a3, a4, a5, a6)                         STR(a1##a2##a3##a4##a5##a6)
#define CAT_STR7(a1, a2, a3, a4, a5, a6, a7)                     STR(a1##a2##a3##a4##a5##a6##a7)
#define CAT_STR8(a1, a2, a3, a4, a5, a6, a7, a8)                 STR(a1##a2##a3##a4##a5##a6##a7##a8)
#define CAT_STR9(a1, a2, a3, a4, a5, a6, a7, a8, a9)             STR(a1##a2##a3##a4##a5##a6##a7##a8##a9)
// ...

#define CAT_STR(...) IDENTITY(APPLY(CONCAT_STRINGIFY_DISPATCH, COUNT(__VA_ARGS__)))(__VA_ARGS__)

CAT_STR(AAA, BBB, CCC)

#define IDENTITY(x)                      x
#define APPLY(m, ...)                    IDENTITY(m(__VA_ARGS__))
#define CONCAT_NONSTRINGIFY_DISPATCH(N)  CONCAT_NONSTRINGIFY##N

#define TO_NONSTRING(X)                  X
#define CONCAT_NONSTRINGIFY1(A)          TO_NONSTRING(A)
#define CONCAT_NONSTRINGIFY2(A, B)       TO_NONSTRING(A##B)
#define CONCAT_NONSTRINGIFY3(A, B, C)    TO_NONSTRING(A##B##C)
#define CONCAT_NONSTRINGIFY4(A, B, C, D) TO_NONSTRING(A##B##C##D)
// ...

#define CONCAT_NONSTRINGIFY(...) \
    IDENTITY(APPLY(CONCAT_NONSTRINGIFY_DISPATCH, COUNT(__VA_ARGS__)))(__VA_ARGS__)
CONCAT_NONSTRINGIFY(AAA, BBB, CCC)
