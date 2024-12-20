/// REFERENCE netlib CBLAS.h header
/// https://www.netlib.org/blas/cblas.h

#pragma once

#ifndef BL_PREFIX
/// Ensure the original function name is provided as an alias to the prefixed function
#define F_ALIAS(fname, prefix_name, ret_type, ...) extern ret_type fname(__VA_ARGS__) __attribute__((weak, alias(STR(prefix_name ## fname))));


/// Compiler over-rideable value for BLAS function prefix
#define BLAS_PREFIX scipy_

/// Ensure the provided functio name expands with the given variadic arguments
#define BL_PREFIX_HELPER1(func_name, ...)  func_name(__VA_ARGS__) {}

/// Ensure the prefix and func-name are concatenated
#define BL_PREFIX_HELPER2(fname, prefix) prefix ## fname

/// Ensure the function name, prefix and arguments are offloaded to the above helpers for expnasion
#define BL_PREFIX_HELPER3(fname, prefix, ret_type, ...) BL_PREFIX_HELPER1(BL_PREFIX_HELPER2(fname, prefix), __VA_ARGS__ ); F_ALIAS(fname, prefix, ret_type, __VA_ARGS__)

/// Ensure the prefix value can be set fully with `-DBLAS_PREFIX=` compiler flag
#define BL_PREFIX(fname, ret_type, ...) ret_type BL_PREFIX_HELPER3(fname, BLAS_PREFIX, ret_type, __VA_ARGS__)

#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
 * ===========================================================================
 * Prototypes for level 1 BLAS functions (complex are recast as routines)
 * ===========================================================================
 */


float BL_PREFIX(sdsdot, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(dsdot, const int N, const float* X, const int incX, const float* Y, const int incY);
float BL_PREFIX(sdot, const int N, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(ddot, const int N, const double* X, const int incX, const double* Y, const int incY);

/*
 * Functions having prefixes Z and C only
 */
void BL_PREFIX(cdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(cdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);
void BL_PREFIX(zdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(zdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS functions (complex are recast as routines)
 * ===========================================================================
 */
 
float BL_PREFIX(sdsdot, const int N, const float alpha, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(dsdot, const int N, const float* X, const int incX, const float* Y, const int incY);
float BL_PREFIX(sdot, const int N, const float* X, const int incX, const float* Y, const int incY);
double BL_PREFIX(ddot, const int N, const double* X, const int incX, const double* Y, const int incY);

/*
 * Functions having prefixes Z and C only
 */
void BL_PREFIX(cdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(cdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);
void BL_PREFIX(zdotu_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotu);
void BL_PREFIX(zdotc_sub, const int N, const void* X, const int incX, const void* Y, const int incY, void* dotc);

/*
 * Functions having prefixes S D SC DZ
 */
float BL_PREFIX(snrm2, const int N, const float* X, const int incX);
float BL_PREFIX(sasum, const int N, const float* X, const int incX);

double BL_PREFIX(dnrm2, const int N, const double* X, const int incX);
double BL_PREFIX(dasum, const int N, const double* X, const int incX);

float BL_PREFIX(scnrm2, const int N, const void* X, const int incX);
float BL_PREFIX(scasum, const int N, const void* X, const int incX);

double BL_PREFIX(dznrm2, const int N, const void* X, const int incX);
double BL_PREFIX(dzasum, const int N, const void* X, const int incX);

/*
 * Functions having standard 4 prefixes (S D C Z)
 */
CBLAS_INDEX BL_PREFIX(isamax, const int N, const float* X, const int incX);
CBLAS_INDEX BL_PREFIX(idamax, const int N, const double* X, const int incX);
CBLAS_INDEX BL_PREFIX(icamax, const int N, const void* X, const int incX);
CBLAS_INDEX BL_PREFIX(izamax, const int N, const void* X, const int incX);

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS routines
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (s, d, c, z)
 */
void BL_PREFIX(sswap, const int N, float* X, const int incX, float* Y, const int incY);
void BL_PREFIX(scopy, const int N, const float* X, const int incX, float* Y, const int incY);
void BL_PREFIX(saxpy, const int N, const float alpha, const float* X, const int incX, float* Y,
                 const int incY);

void BL_PREFIX(dswap, const int N, double* X, const int incX, double* Y, const int incY);
void BL_PREFIX(dcopy, const int N, const double* X, const int incX, double* Y, const int incY);
void BL_PREFIX(daxpy, const int N, const double alpha, const double* X, const int incX, double* Y,
                 const int incY);

void BL_PREFIX(cswap, const int N, void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(ccopy, const int N, const void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(caxpy, const int N, const void* alpha, const void* X, const int incX, void* Y,
                 const int incY);

void BL_PREFIX(zswap, const int N, void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(zcopy, const int N, const void* X, const int incX, void* Y, const int incY);
void BL_PREFIX(zaxpy, const int N, const void* alpha, const void* X, const int incX, void* Y,
                 const int incY);

/*
 * Routines with S and D prefix only
 */
void BL_PREFIX(srotg, float* a, float* b, float* c, float* s);
void BL_PREFIX(srotmg, float* d1, float* d2, float* b1, const float b2, float* P);
void BL_PREFIX(srot, const int N, float* X, const int incX, float* Y, const int incY, const float c,
                const float s);
void BL_PREFIX(srotm, const int N, float* X, const int incX, float* Y, const int incY, const float* P);

void BL_PREFIX(drotg, double* a, double* b, double* c, double* s);
void BL_PREFIX(drotmg, double* d1, double* d2, double* b1, const double b2, double* P);
void BL_PREFIX(drot, const int N, double* X, const int incX, double* Y, const int incY, const double c,
                const double s);
void BL_PREFIX(drotm, const int N, double* X, const int incX, double* Y, const int incY,
                 const double* P);

/*
 * Routines with S D C Z CS and ZD prefixes
 */
void BL_PREFIX(sscal, const int N, const float alpha, float* X, const int incX);
void BL_PREFIX(dscal, const int N, const double alpha, double* X, const int incX);
void BL_PREFIX(cscal, const int N, const void* alpha, void* X, const int incX);
void BL_PREFIX(zscal, const int N, const void* alpha, void* X, const int incX);
void BL_PREFIX(csscal, const int N, const float alpha, void* X, const int incX);
void BL_PREFIX(zdscal, const int N, const double alpha, void* X, const int incX);

/*
 * ===========================================================================
 * Prototypes for level 2 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void BL_PREFIX(sgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const float alpha, const float* A, const int lda, const float* X,
                 const int incX, const float beta, float* Y, const int incY);
void BL_PREFIX(sgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const float alpha, const float* A,
                 const int lda, const float* X, const int incX, const float beta, float* Y,
                 const int incY);
void BL_PREFIX(strmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(stbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(stpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* Ap, float* X, const int incX);
void BL_PREFIX(strsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(stbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const float* A, const int lda, float* X, const int incX);
void BL_PREFIX(stpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const float* Ap, float* X, const int incX);

void BL_PREFIX(dgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const double alpha, const double* A, const int lda, const double* X,
                 const int incX, const double beta, double* Y, const int incY);
void BL_PREFIX(dgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const double alpha, const double* A,
                 const int lda, const double* X, const int incX, const double beta, double* Y,
                 const int incY);
void BL_PREFIX(dtrmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(dtbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(dtpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* Ap, double* X, const int incX);
void BL_PREFIX(dtrsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(dtbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const double* A, const int lda, double* X, const int incX);
void BL_PREFIX(dtpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const double* Ap, double* X, const int incX);

void BL_PREFIX(cgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const void* alpha, const void* A, const int lda, const void* X,
                 const int incX, const void* beta, void* Y, const int incY);
void BL_PREFIX(cgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const void* alpha, const void* A,
                 const int lda, const void* X, const int incX, const void* beta, void* Y,
                 const int incY);
void BL_PREFIX(ctrmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ctbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ctpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);
void BL_PREFIX(ctrsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ctbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ctpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);

void BL_PREFIX(zgemv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const void* alpha, const void* A, const int lda, const void* X,
                 const int incX, const void* beta, void* Y, const int incY);
void BL_PREFIX(zgbmv, const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M,
                 const int N, const int KL, const int KU, const void* alpha, const void* A,
                 const int lda, const void* X, const int incX, const void* beta, void* Y,
                 const int incY);
void BL_PREFIX(ztrmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ztbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ztpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);
void BL_PREFIX(ztrsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ztbsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const int K, const void* A, const int lda, void* X, const int incX);
void BL_PREFIX(ztpsv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N,
                 const void* Ap, void* X, const int incX);

/*
 * Routines with S and D prefixes only
 */
void BL_PREFIX(ssymv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* A, const int lda, const float* X, const int incX,
                 const float beta, float* Y, const int incY);
void BL_PREFIX(ssbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const float alpha, const float* A, const int lda, const float* X, const int incX,
                 const float beta, float* Y, const int incY);
void BL_PREFIX(sspmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* Ap, const float* X, const int incX,
                 const float beta, float* Y, const int incY);
void BL_PREFIX(sger, const enum CBLAS_ORDER order, const int M, const int N, const float alpha,
                const float* X, const int incX, const float* Y, const int incY, float* A,
                const int lda);
void BL_PREFIX(ssyr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const float* X, const int incX, float* A, const int lda);
void BL_PREFIX(sspr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const float* X, const int incX, float* Ap);
void BL_PREFIX(ssyr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* X, const int incX, const float* Y, const int incY,
                 float* A, const int lda);
void BL_PREFIX(sspr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const float alpha, const float* X, const int incX, const float* Y, const int incY,
                 float* A);

void BL_PREFIX(dsymv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* A, const int lda, const double* X,
                 const int incX, const double beta, double* Y, const int incY);
void BL_PREFIX(dsbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const double alpha, const double* A, const int lda, const double* X,
                 const int incX, const double beta, double* Y, const int incY);
void BL_PREFIX(dspmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* Ap, const double* X, const int incX,
                 const double beta, double* Y, const int incY);
void BL_PREFIX(dger, const enum CBLAS_ORDER order, const int M, const int N, const double alpha,
                const double* X, const int incX, const double* Y, const int incY, double* A,
                const int lda);
void BL_PREFIX(dsyr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const double* X, const int incX, double* A, const int lda);
void BL_PREFIX(dspr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const double* X, const int incX, double* Ap);
void BL_PREFIX(dsyr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* X, const int incX, const double* Y,
                 const int incY, double* A, const int lda);
void BL_PREFIX(dspr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const double alpha, const double* X, const int incX, const double* Y,
                 const int incY, double* A);

/*
 * Routines with C and Z prefixes only
 */
void BL_PREFIX(chemv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(chbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(chpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* Ap, const void* X, const int incX, const void* beta,
                 void* Y, const int incY);
void BL_PREFIX(cgeru, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(cgerc, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(cher, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const void* X, const int incX, void* A, const int lda);
void BL_PREFIX(chpr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const float alpha, const void* X, const int incX, void* A);
void BL_PREFIX(cher2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* A, const int lda);
void BL_PREFIX(chpr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* Ap);

void BL_PREFIX(zhemv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(zhbmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* X, const int incX,
                 const void* beta, void* Y, const int incY);
void BL_PREFIX(zhpmv, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* Ap, const void* X, const int incX, const void* beta,
                 void* Y, const int incY);
void BL_PREFIX(zgeru, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(zgerc, const enum CBLAS_ORDER order, const int M, const int N, const void* alpha,
                 const void* X, const int incX, const void* Y, const int incY, void* A,
                 const int lda);
void BL_PREFIX(zher, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const void* X, const int incX, void* A, const int lda);
void BL_PREFIX(zhpr, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const double alpha, const void* X, const int incX, void* A);
void BL_PREFIX(zher2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                 const void* alpha, const void* X, const int incX, const void* Y, const int incY,
                 void* A, const int lda);
void BL_PREFIX(zhpr2, const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
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
void BL_PREFIX(sgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const float alpha, const float* A, const int lda, const float* B, const int ldb,
                 const float beta, float* C, const int ldc);
void BL_PREFIX(ssymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const float alpha,
                 const float* A, const int lda, const float* B, const int ldb, const float beta,
                 float* C, const int ldc);
void BL_PREFIX(ssyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha,
                 const float* A, const int lda, const float beta, float* C, const int ldc);
void BL_PREFIX(ssyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha,
                  const float* A, const int lda, const float* B, const int ldb, const float beta,
                  float* C, const int ldc);
void BL_PREFIX(strmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const float alpha,
                 const float* A, const int lda, float* B, const int ldb);
void BL_PREFIX(strsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const float alpha,
                 const float* A, const int lda, float* B, const int ldb);

void BL_PREFIX(dgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const double alpha, const double* A, const int lda, const double* B, const int ldb,
                 const double beta, double* C, const int ldc);
void BL_PREFIX(dsymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const double alpha,
                 const double* A, const int lda, const double* B, const int ldb, const double beta,
                 double* C, const int ldc);
void BL_PREFIX(dsyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha,
                 const double* A, const int lda, const double beta, double* C, const int ldc);
void BL_PREFIX(dsyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha,
                  const double* A, const int lda, const double* B, const int ldb, const double beta,
                  double* C, const int ldc);
void BL_PREFIX(dtrmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const double alpha,
                 const double* A, const int lda, double* B, const int ldb);
void BL_PREFIX(dtrsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const double alpha,
                 const double* A, const int lda, double* B, const int ldb);

void BL_PREFIX(cgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* B, const int ldb,
                 const void* beta, void* C, const int ldc);
void BL_PREFIX(csymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(csyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                 const void* A, const int lda, const void* beta, void* C, const int ldc);
void BL_PREFIX(csyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const void* beta,
                  void* C, const int ldc);
void BL_PREFIX(ctrmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);
void BL_PREFIX(ctrsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);

void BL_PREFIX(zgemm, const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K,
                 const void* alpha, const void* A, const int lda, const void* B, const int ldb,
                 const void* beta, void* C, const int ldc);
void BL_PREFIX(zsymm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(zsyrk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                 const void* A, const int lda, const void* beta, void* C, const int ldc);
void BL_PREFIX(zsyr2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const void* beta,
                  void* C, const int ldc);
void BL_PREFIX(ztrmm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);
void BL_PREFIX(ztrsm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N, const void* alpha,
                 const void* A, const int lda, void* B, const int ldb);

/*
 * Routines with prefixes C and Z only
 */
void BL_PREFIX(chemm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(cherk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha,
                 const void* A, const int lda, const float beta, void* C, const int ldc);
void BL_PREFIX(cher2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const float beta,
                  void* C, const int ldc);

void BL_PREFIX(zhemm, const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N, const void* alpha,
                 const void* A, const int lda, const void* B, const int ldb, const void* beta,
                 void* C, const int ldc);
void BL_PREFIX(zherk, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha,
                 const void* A, const int lda, const double beta, void* C, const int ldc);
void BL_PREFIX(zher2k, const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void* alpha,
                  const void* A, const int lda, const void* B, const int ldb, const double beta,
                  void* C, const int ldc);

void BL_PREFIX(xerbla, int p, const char* rout, const char* form, ...);

#ifdef __cplusplus
}
#endif
#endif

