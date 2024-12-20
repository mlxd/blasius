/// REFERENCE netlib CBLAS.h header
/// https://www.netlib.org/blas/cblas.h

#pragma once

/// Compiler over-rideable value for BLAS function prefix
#ifndef BLAS_PREFIX
#define BLAS_PREFIX scipy_
#endif

/// Compiler over-rideable value for BLAS function suffix
#ifndef BLAS_SUFFIX
#define BLAS_SUFFIX //_64
#endif

/// Ensure the provided functio name expands with the given variadic arguments
#define BL_PREFIX_HELPER1(func_name, ...)  func_name(__VA_ARGS__) {}

/// Ensure the prefix and func-name are concatenated
#define BL_PREFIX_HELPER2(fname, prefix, suffix) prefix ## fname ## suffix

/// Ensure the function name, prefix and arguments are offloaded to the above helpers for expansion
#define BL_PREFIX_HELPER3(fname, prefix, suffix, ret_type, ...) BL_PREFIX_HELPER1(BL_PREFIX_HELPER2(fname, prefix, suffix), __VA_ARGS__ ); F_ALIAS(fname, prefix, suffix, ret_type, __VA_ARGS__)

/// Ensure the prefix value can be set fully with `-DBLAS_PREFIX=` compiler flag
#define BL_PREFIX(fname, ret_type, ...) ret_type __attribute__((weak)) BL_PREFIX_HELPER3(fname, BLAS_PREFIX, BLAS_SUFFIX, ret_type, __VA_ARGS__)

/// Ensure the original function name is provided as an alias to the prefixed function
#define F_ALIAS(fname, prefix_name, suffix, ret_type, ...) extern ret_type fname(__VA_ARGS__) __attribute__((weak, alias(STR(prefix_name ## fname ## suffix))));

#ifdef __cplusplus
extern "C" {
#endif


/*
 * ===========================================================================
 * Prototypes for level 1 BLAS functions (complex are recast as routines)
 * ===========================================================================
 */


float BL_PREFIX(cblas_sdsdot, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(cblas_dsdot, const int N, const float* X, const int incX, const float* Y, const int incY);
float BL_PREFIX(cblas_sdot, const int N, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(cblas_ddot, const int N, const double* X, const int incX, const double* Y, const int incY);

/*
 * Functions having prefixes Z and C only
 */
void BL_PREFIX(cblas_cdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(cblas_cdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);
void BL_PREFIX(cblas_zdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(cblas_zdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS functions (complex are recast as routines)
 * ===========================================================================
 */
 
float BL_PREFIX(cblas_sdsdot, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(cblas_dsdot, const int N, const float* X, const int incX, const float* Y, const int incY);
float BL_PREFIX(cblas_sdot, const int N, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(cblas_ddot, const int N, const double* X, const int incX, const double* Y, const int incY);

/*
 * Functions having prefixes Z and C only
 */
void BL_PREFIX(cblas_cdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(cblas_cdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);
void BL_PREFIX(cblas_zdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(cblas_zdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);

/*
 * Functions having prefixes S D SC DZ
 */
float BL_PREFIX(cblas_snrm2, const int N, const float* X, const int incX);
float BL_PREFIX(cblas_sasum, const int N, const float* X, const int incX);

double BL_PREFIX(cblas_dnrm2, const int N, const double* X, const int incX);
double BL_PREFIX(cblas_dasum, const int N, const double* X, const int incX);

float BL_PREFIX(cblas_scnrm2, const int N, const void* X, const int incX);
float BL_PREFIX(cblas_scasum, const int N, const void* X, const int incX);

double BL_PREFIX(cblas_dznrm2, const int N, const void* X, const int incX);
double BL_PREFIX(cblas_dzasum, const int N, const void* X, const int incX);

/*
 * Functions having standard 4 prefixes (S D C Z)
 */
CBLAS_INDEX BL_PREFIX(cblas_isamax, const int N, const float* X, const int incX);
CBLAS_INDEX BL_PREFIX(cblas_idamax, const int N, const double* X, const int incX);
CBLAS_INDEX BL_PREFIX(cblas_icamax, const int N, const void* X, const int incX);
CBLAS_INDEX BL_PREFIX(cblas_izamax, const int N, const void* X, const int incX);

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS routines
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (s, d, c, z)
 */
void BL_PREFIX(cblas_sswap, const int N, float* X, const int incX, float* Y, const int incY);
void BL_PREFIX(cblas_scopy, const int N, const float* X, const int incX, float* Y, const int incY);
void BL_PREFIX(cblas_saxpy, const int N, const float alpha, const float* X, const int incX, float* Y,
                 const int incY);

void BL_PREFIX(cblas_dswap, const int N, double* X, const int incX, double* Y, const int incY);
void BL_PREFIX(cblas_dcopy, const int N, const double* X, const int incX, double* Y, const int incY);
void BL_PREFIX(cblas_daxpy, const int N, const double alpha, const double* X, const int incX, double* Y,
                 const int incY);

void BL_PREFIX(cblas_cswap, const int N, void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(cblas_ccopy, const int N, const void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(cblas_caxpy, const int N, const void* alpha, const void* X, const int incX, void* Y,
                 const int incY);

void BL_PREFIX(cblas_zswap, const int N, void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(cblas_zcopy, const int N, const void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(cblas_zaxpy, const int N, const void* alpha, const void* X, const int incX, void* Y,
                 const int incY);

/*
 * Routines with S and D prefix only
 */
void BL_PREFIX(cblas_srotg, float* a, float* b, float* c, float* s);
void BL_PREFIX(cblas_srotmg, float* d1, float* d2, float* b1, const float b2, float* P);
void BL_PREFIX(cblas_srot, const int N, float* X, const int incX, float* Y, const int incY, const float c,
                const float s);
void BL_PREFIX(cblas_srotm, const int N, float* X, const int incX, float* Y, const int incY, const float* P);

void BL_PREFIX(cblas_drotg, double* a, double* b, double* c, double* s);
void BL_PREFIX(cblas_drotmg, double* d1, double* d2, double* b1, const double b2, double* P);
void BL_PREFIX(cblas_drot, const int N, double* X, const int incX, double* Y, const int incY, const double c,
                const double s);
void BL_PREFIX(cblas_drotm, const int N, double* X, const int incX, double* Y, const int incY,
                 const double* P);

/*
 * Routines with S D C Z CS and ZD prefixes
 */
void BL_PREFIX(cblas_sscal, const int N, const float alpha, float* X, const int incX);
void BL_PREFIX(cblas_dscal, const int N, const double alpha, double* X, const int incX);
void BL_PREFIX(cblas_cscal, const int N, const void* alpha, void* X, const int incX);
void BL_PREFIX(cblas_zscal, const int N, const void* alpha, void* X, const int incX);
void BL_PREFIX(cblas_csscal, const int N, const float alpha, void* X, const int incX);
void BL_PREFIX(cblas_zdscal, const int N, const double alpha, void* X, const int incX);

/*
 * ===========================================================================
 * Prototypes for level 2 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void BL_PREFIX(cblas_sgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const float alpha, const float* A, const int lda, const float* X,
                 const int incX, const float beta, float* Y, const int incY);
void BL_PREFIX(cblas_sgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const float alpha, const float* A,
                 const int lda, const float* X, const int incX, const float beta, float* Y,
                 const int incY);
void BL_PREFIX(cblas_strmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(cblas_stbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(cblas_stpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* Ap, float* X, const int incX);
void BL_PREFIX(cblas_strsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(cblas_stbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(cblas_stpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* Ap, float* X, const int incX);

void BL_PREFIX(cblas_dgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const double alpha, const double* A, const int lda, const double* X,
                 const int incX, const double beta, double* Y, const int incY);
void BL_PREFIX(cblas_dgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const double alpha, const double* A,
                 const int lda, const double* X, const int incX, const double beta, double* Y,
                 const int incY);
void BL_PREFIX(cblas_dtrmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(cblas_dtbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(cblas_dtpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* Ap, double* X, const int incX);
void BL_PREFIX(cblas_dtrsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(cblas_dtbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(cblas_dtpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* Ap, double* X, const int incX);

void BL_PREFIX(cblas_cgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const void* alpha, const void* A, const int lda, const void* X,
                 const int incX, const void* beta, void* Y, const int incY);
void BL_PREFIX(cblas_cgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const void* alpha, const void* A,
                 const int lda, const void* X, const int incX, const void* beta, void* Y,
                 const int incY);
void BL_PREFIX(cblas_ctrmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ctbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ctpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);
void BL_PREFIX(cblas_ctrsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ctbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ctpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);

void BL_PREFIX(cblas_zgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const void* alpha, const void* A, const int lda, const void* X,
                 const int incX, const void* beta, void* Y, const int incY);
void BL_PREFIX(cblas_zgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const void* alpha, const void* A,
                 const int lda, const void* X, const int incX, const void* beta, void* Y,
                 const int incY);
void BL_PREFIX(cblas_ztrmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ztbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ztpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);
void BL_PREFIX(cblas_ztrsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ztbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(cblas_ztpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);

/*
 * Routines with S and D prefixes only
 */
void BL_PREFIX(cblas_ssymv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* A, const int lda, const float* X, const int incX,
                 const float beta, float* Y, const int incY);
void BL_PREFIX(cblas_ssbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const float alpha, const float* A, const int lda, const float* X, const int incX,
                 const float beta, float* Y, const int incY);
void BL_PREFIX(cblas_sspmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* Ap, const float* X, const int incX,
                 const float beta, float* Y, const int incY);
void BL_PREFIX(cblas_sger, const enum CBLAS_ORDER order, const int M, const int N, const float alpha,
                const float* X, const int incX, const float* Y, const int incY, float* A,
                const int lda);
void BL_PREFIX(cblas_ssyr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const float* X, const int incX, float* A, const int lda);
void BL_PREFIX(cblas_sspr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const float* X, const int incX, float* Ap);
void BL_PREFIX(cblas_ssyr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* X, const int incX, const float* Y, const int incY,
                 float* A, const int lda);
void BL_PREFIX(cblas_sspr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* X, const int incX, const float* Y, const int incY,
                 float* A);

void BL_PREFIX(cblas_dsymv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* A, const int lda, const double* X,
                 const int incX, const double beta, double* Y, const int incY);
void BL_PREFIX(cblas_dsbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const double alpha, const double* A, const int lda, const double* X,
                 const int incX, const double beta, double* Y, const int incY);
void BL_PREFIX(cblas_dspmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* Ap, const double* X, const int incX,
                 const double beta, double* Y, const int incY);
void BL_PREFIX(cblas_dger, const enum CBLAS_ORDER order, const int M, const int N, const double alpha,
                const double* X, const int incX, const double* Y, const int incY, double* A,
                const int lda);
void BL_PREFIX(cblas_dsyr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const double* X, const int incX, double* A, const int lda);
void BL_PREFIX(cblas_dspr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const double* X, const int incX, double* Ap);
void BL_PREFIX(cblas_dsyr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* X, const int incX, const double* Y,
                 const int incY, double* A, const int lda);
void BL_PREFIX(cblas_dspr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* X, const int incX, const double* Y,
                 const int incY, double* A);

/*
 * Routines with C and Z prefixes only
 */
void BL_PREFIX(cblas_chemv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(cblas_chbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(cblas_chpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* Ap, const void* X, const int incX, const void* beta,
                 void* Y, const int incY);
void BL_PREFIX(cblas_cgeru, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(cblas_cgerc, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(cblas_cher, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const void* X, const int incX, void* A, const int lda);
void BL_PREFIX(cblas_chpr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const void* X, const int incX, void* A);
void BL_PREFIX(cblas_cher2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* A, const int lda);
void BL_PREFIX(cblas_chpr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* Ap);

void BL_PREFIX(cblas_zhemv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(cblas_zhbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(cblas_zhpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* Ap, const void* X, const int incX, const void* beta,
                 void* Y, const int incY);
void BL_PREFIX(cblas_zgeru, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(cblas_zgerc, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(cblas_zher, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const void* X, const int incX, void* A, const int lda);
void BL_PREFIX(cblas_zhpr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const void* X, const int incX, void* A);
void BL_PREFIX(cblas_zher2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* A, const int lda);
void BL_PREFIX(cblas_zhpr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* Ap);

/*
 * ===========================================================================
 * Prototypes for level 3 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void BL_PREFIX(cblas_sgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const float alpha, const float* A, const int lda, const float* B, const int ldb,
                 const float beta, float* C, const int ldc);
void BL_PREFIX(cblas_ssymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const float alpha,
                 const float* A, const int lda, const float* B, const int ldb, const float beta,
                 float* C, const int ldc);
void BL_PREFIX(cblas_ssyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha,
                 const float* A, const int lda, const float beta, float* C, const int ldc);
void BL_PREFIX(cblas_ssyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha,
                  const float* A, const int lda, const float* B, const int ldb, const float beta,
                  float* C, const int ldc);
void BL_PREFIX(cblas_strmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const float alpha,
                 const float* A, const int lda, float* B, const int ldb);
void BL_PREFIX(cblas_strsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const float alpha,
                 const float* A, const int lda, float* B, const int ldb);

void BL_PREFIX(cblas_dgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const double alpha, const double* A, const int lda, const double* B, const int ldb,
                 const double beta, double* C, const int ldc);
void BL_PREFIX(cblas_dsymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const double alpha,
                 const double* A, const int lda, const double* B, const int ldb, const double beta,
                 double* C, const int ldc);
void BL_PREFIX(cblas_dsyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha,
                 const double* A, const int lda, const double beta, double* C, const int ldc);
void BL_PREFIX(cblas_dsyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha,
                  const double* A, const int lda, const double* B, const int ldb, const double beta,
                  double* C, const int ldc);
void BL_PREFIX(cblas_dtrmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const double alpha,
                 const double* A, const int lda, double* B, const int ldb);
void BL_PREFIX(cblas_dtrsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const double alpha,
                 const double* A, const int lda, double* B, const int ldb);

void BL_PREFIX(cblas_cgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* B, const int ldb,
                 const void* beta, void* C, const int ldc);
void BL_PREFIX(cblas_csymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(cblas_csyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                 const void* A, const int lda, const void* beta, void* C, const int ldc);
void BL_PREFIX(cblas_csyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const void* beta,
                  void* C, const int ldc);
void BL_PREFIX(cblas_ctrmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);
void BL_PREFIX(cblas_ctrsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);

void BL_PREFIX(cblas_zgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* B, const int ldb,
                 const void* beta, void* C, const int ldc);
void BL_PREFIX(cblas_zsymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(cblas_zsyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                 const void* A, const int lda, const void* beta, void* C, const int ldc);
void BL_PREFIX(cblas_zsyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const void* beta,
                  void* C, const int ldc);
void BL_PREFIX(cblas_ztrmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);
void BL_PREFIX(cblas_ztrsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);

/*
 * Routines with prefixes C and Z only
 */
void BL_PREFIX(cblas_chemm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(cblas_cherk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha,
                 const void* A, const int lda, const float beta, void* C, const int ldc);
void BL_PREFIX(cblas_cher2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const float beta,
                  void* C, const int ldc);

void BL_PREFIX(cblas_zhemm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(cblas_zherk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha,
                 const void* A, const int lda, const double beta, void* C, const int ldc);
void BL_PREFIX(cblas_zher2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const double beta,
                  void* C, const int ldc);

void BL_PREFIX(cblas_xerbla, int p, const char* rout, const char* form, ...);

#ifdef __cplusplus
}
#endif
#endif

