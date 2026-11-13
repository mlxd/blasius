/* rmtest on Accelerate: CBLAS and the two Fortran trsm calls bound to the
 * $NEWLAPACK family by asm label. ACC_SFX is "$NEWLAPACK" or "$NEWLAPACK$ILP64".
 * The LAPACK_* macros reach the same family through lapacke_mangling.h. */
enum CBLAS_ORDER { CblasRowMajor = 101, CblasColMajor = 102 };
enum CBLAS_TRANSPOSE { CblasNoTrans = 111, CblasTrans = 112, CblasConjTrans = 113 };
void cblas_dgemm(enum CBLAS_ORDER, enum CBLAS_TRANSPOSE, enum CBLAS_TRANSPOSE, lapack_int, lapack_int, lapack_int, double, const double*,
                 lapack_int, const double*, lapack_int, double, double*, lapack_int) __asm("_cblas_dgemm" ACC_SFX);
void cblas_zgemm(enum CBLAS_ORDER, enum CBLAS_TRANSPOSE, enum CBLAS_TRANSPOSE, lapack_int, lapack_int, lapack_int, const void*, const void*,
                 lapack_int, const void*, lapack_int, const void*, void*, lapack_int) __asm("_cblas_zgemm" ACC_SFX);
void cblas_dswap(lapack_int, double*, lapack_int, double*, lapack_int) __asm("_cblas_dswap" ACC_SFX);
void acc_dtrsm(const char*, const char*, const char*, const char*, const lapack_int*, const lapack_int*, const double*, const double*,
               const lapack_int*, double*, const lapack_int*, size_t, size_t, size_t, size_t) __asm("_dtrsm" ACC_SFX);
void acc_ztrsm(const char*, const char*, const char*, const char*, const lapack_int*, const lapack_int*, const void*, const void*,
               const lapack_int*, void*, const lapack_int*, size_t, size_t, size_t, size_t) __asm("_ztrsm" ACC_SFX);
#define LAPACK_dtrsm_base acc_dtrsm
#define LAPACK_ztrsm_base acc_ztrsm
#define openblas_get_config() ("Accelerate " ACC_SFX " (netlib LAPACKE 3.12.1 reference on the same family)")
