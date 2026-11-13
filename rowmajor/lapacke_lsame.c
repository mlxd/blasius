/* Replaces LAPACKE's utils/lapacke_lsame.c, which calls Fortran lsame_; Accelerate
 * has no lsame$NEWLAPACK (scipy binds the legacy one), and this keeps the reference self-contained. */
#include "lapacke_utils.h"
lapack_logical LAPACKE_lsame(char ca, char cb) { return (ca | 0x20) == (cb | 0x20); }
