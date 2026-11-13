/* Row-major LAPACKE "no-transpose" trick checker.
 * Reference = netlib LAPACKE_x(LAPACK_ROW_MAJOR, ...) (transposes internally).
 * Trick     = direct Fortran-ABI call on the caller's row-major buffer, read as
 *             the column-major transpose, with argument re-expression.
 * Reports max |diff| and whether the outputs are bit-identical.            */
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lapacke.h>
#ifdef ACC_SFX /* macOS smoke test: Accelerate $NEWLAPACK family */
#include "acc_shim.h"
#else
#include <cblas.h>
void dtrsm_(const char*,const char*,const char*,const char*,const lapack_int*,const lapack_int*,const double*,const double*,const lapack_int*,double*,const lapack_int*,size_t,size_t,size_t,size_t);
void ztrsm_(const char*,const char*,const char*,const char*,const lapack_int*,const lapack_int*,const void*,const void*,const lapack_int*,void*,const lapack_int*,size_t,size_t,size_t,size_t);
#define LAPACK_dtrsm_base dtrsm_
#define LAPACK_ztrsm_base ztrsm_
#endif

typedef double complex zc;
/* Build: gcc -O2 -std=gnu11 -o rmtest rmtest.c -llapacke -llapack -lopenblas -lm
 * Run:   OPENBLAS_NUM_THREADS=1 ./rmtest [n]                                   */
static unsigned long long rs = 88172645463325252ULL;
static double rnd(void) { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (double)(rs >> 11) / 9007199254740992.0 * 2 - 1; }
static void fill_d(double *a, size_t k) { for (size_t i = 0; i < k; i++) a[i] = rnd(); }
static void fill_z(zc *a, size_t k) { for (size_t i = 0; i < k; i++) a[i] = rnd() + I * rnd(); }
static double md_d(const double *a, const double *b, size_t k) { double m = 0; for (size_t i = 0; i < k; i++) m = fmax(m, fabs(a[i] - b[i])); return m; }
static double md_z(const zc *a, const zc *b, size_t k) { double m = 0; for (size_t i = 0; i < k; i++) m = fmax(m, cabs(a[i] - b[i])); return m; }
static int bw(const void *a, const void *b, size_t bytes) { return memcmp(a, b, bytes) == 0; }
static void rep(const char *name, double d, int bit) { printf("  %-58s maxdiff=%.2e  bitwise=%s\n", name, d, bit ? "YES" : "no"); }
static char flip(char u) { return (u == 'L' || u == 'l') ? 'U' : 'L'; }
static void trd(int n, double *a, int lda) { for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) { double t = a[i*lda+j]; a[i*lda+j] = a[j*lda+i]; a[j*lda+i] = t; } }
static void trz(int n, zc *a, int lda, int cj) { for (int i = 0; i < n; i++) { if (cj) a[i*lda+i] = conj(a[i*lda+i]); for (int j = i + 1; j < n; j++) { zc t = a[i*lda+j]; a[i*lda+j] = cj ? conj(a[j*lda+i]) : a[j*lda+i]; a[j*lda+i] = cj ? conj(t) : t; } } }
/* compare column j of row-major X (n x c, ld) up to sign/phase */
static double gauge_d(int r, int c, const double *x, int ldx, const double *y, int ldy, int *nflip) {
  double m = 0; *nflip = 0;
  for (int j = 0; j < c; j++) { double dp = 0; for (int i = 0; i < r; i++) dp += x[i*ldx+j] * y[i*ldy+j];
    double s = dp < 0 ? -1 : 1; if (s < 0) (*nflip)++;
    for (int i = 0; i < r; i++) m = fmax(m, fabs(x[i*ldx+j] - s * y[i*ldy+j])); }
  return m; }
static double gauge_z(int r, int c, const zc *x, int ldx, const zc *y, int ldy, int *nflip) {
  double m = 0; *nflip = 0;
  for (int j = 0; j < c; j++) { zc dp = 0; for (int i = 0; i < r; i++) dp += conj(y[i*ldy+j]) * x[i*ldx+j];
    zc ph = dp / cabs(dp); if (cabs(ph - 1) > 1e-12) (*nflip)++;
    for (int i = 0; i < r; i++) m = fmax(m, cabs(x[i*ldx+j] - ph * y[i*ldy+j])); }
  return m; }
/* rows of row-major X compared as vectors: used for VT (row i = right sing. vector i) */
static double gauge_rows_d(int r, int c, const double *x, int ldx, const double *y, int ldy, int *nflip) {
  double m = 0; *nflip = 0;
  for (int i = 0; i < r; i++) { double dp = 0; for (int j = 0; j < c; j++) dp += x[i*ldx+j] * y[i*ldy+j];
    double s = dp < 0 ? -1 : 1; if (s < 0) (*nflip)++;
    for (int j = 0; j < c; j++) m = fmax(m, fabs(x[i*ldx+j] - s * y[i*ldy+j])); }
  return m; }

static void t_potrf(int n) {
  printf("potrf (n=%d): uplo flip, Fortran call on row-major buffer\n", n);
  for (int u = 0; u < 2; u++) { char uplo = u ? 'U' : 'L';
    double *g = malloc(sizeof(double)*n*n), *a = malloc(sizeof(double)*n*n), *b = malloc(sizeof(double)*n*n);
    fill_d(g, n*n); cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, n, n, 1, g, n, g, n, 0, a, n);
    for (int i = 0; i < n; i++) a[i*n+i] += n;
    memcpy(b, a, sizeof(double)*n*n);
    LAPACKE_dpotrf(LAPACK_ROW_MAJOR, uplo, n, a, n);
    char fu = flip(uplo); lapack_int nn = n, info; LAPACK_dpotrf(&fu, &nn, b, &nn, &info);
    char nm[64]; snprintf(nm, 64, "dpotrf uplo=%c (whole buffer incl. untouched triangle)", uplo);
    rep(nm, md_d(a, b, n*n), bw(a, b, sizeof(double)*n*n)); free(g); free(a); free(b); }
  for (int u = 0; u < 2; u++) { char uplo = u ? 'U' : 'L';
    zc *g = malloc(sizeof(zc)*n*n), *a = malloc(sizeof(zc)*n*n), *b = malloc(sizeof(zc)*n*n); zc one = 1, zero = 0;
    fill_z(g, n*n); cblas_zgemm(CblasRowMajor, CblasNoTrans, CblasConjTrans, n, n, n, &one, g, n, g, n, &zero, a, n);
    for (int i = 0; i < n; i++) a[i*n+i] += n;
    memcpy(b, a, sizeof(zc)*n*n);
    LAPACKE_zpotrf(LAPACK_ROW_MAJOR, uplo, n, a, n);
    char fu = flip(uplo); lapack_int nn = n, info; LAPACK_zpotrf(&fu, &nn, b, &nn, &info);
    char nm[64]; snprintf(nm, 64, "zpotrf uplo=%c (no conjugation applied)", uplo);
    rep(nm, md_z(a, b, n*n), bw(a, b, sizeof(zc)*n*n)); free(g); free(a); free(b); }
}

static void t_geqrf(int m, int n) {
  int k = m < n ? m : n; lapack_int M = n, N = m, LDA = n, info, lw = -1;
  printf("geqrf (m=%d n=%d): ?gelqf on buffer (M=n,N=m)\n", m, n);
  { double *a = malloc(sizeof(double)*m*n), *b = malloc(sizeof(double)*m*n), *t1 = malloc(sizeof(double)*k), *t2 = malloc(sizeof(double)*k), q;
    fill_d(a, m*n); memcpy(b, a, sizeof(double)*m*n);
    LAPACKE_dgeqrf(LAPACK_ROW_MAJOR, m, n, a, n, t1);
    lw = -1; LAPACK_dgelqf(&M, &N, b, &LDA, t2, &q, &lw, &info); lw = (lapack_int)q; double *w = malloc(sizeof(double)*lw);
    LAPACK_dgelqf(&M, &N, b, &LDA, t2, w, &lw, &info);
    rep("dgeqrf A (R + reflectors)", md_d(a, b, m*n), bw(a, b, sizeof(double)*m*n)); rep("dgeqrf tau", md_d(t1, t2, k), bw(t1, t2, sizeof(double)*k));
    free(a); free(b); free(t1); free(t2); free(w); }
  { zc *a = malloc(sizeof(zc)*m*n), *b = malloc(sizeof(zc)*m*n), *t1 = malloc(sizeof(zc)*k), *t2 = malloc(sizeof(zc)*k), q;
    fill_z(a, m*n); memcpy(b, a, sizeof(zc)*m*n);
    LAPACKE_zgeqrf(LAPACK_ROW_MAJOR, m, n, a, n, t1);
    lw = -1; LAPACK_zgelqf(&M, &N, b, &LDA, t2, &q, &lw, &info); lw = (lapack_int)creal(q); zc *w = malloc(sizeof(zc)*lw);
    LAPACK_zgelqf(&M, &N, b, &LDA, t2, w, &lw, &info);
    rep("zgeqrf A (R + reflectors)", md_z(a, b, m*n), bw(a, b, sizeof(zc)*m*n)); rep("zgeqrf tau, raw from zgelqf", md_z(t1, t2, k), 0);
    for (int i = 0; i < k; i++) t2[i] = conj(t2[i]);
    rep("zgeqrf tau, after conj(tau)", md_z(t1, t2, k), bw(t1, t2, sizeof(zc)*k));
    free(a); free(b); free(t1); free(t2); free(w); }
}

static void t_orgqr(int m, int n, int k) {
  printf("orgqr/ungqr (m=%d n=%d k=%d): ?orglq/?unglq on buffer (M=n,N=m,K=k)\n", m, n, k);
  lapack_int M = n, N = m, K = k, LDA = n, info, lw;
  { double *a = malloc(sizeof(double)*m*n), *b = malloc(sizeof(double)*m*n), *t = malloc(sizeof(double)*n), q;
    fill_d(a, m*n); LAPACKE_dgeqrf(LAPACK_ROW_MAJOR, m, n, a, n, t); memcpy(b, a, sizeof(double)*m*n);
    LAPACKE_dorgqr(LAPACK_ROW_MAJOR, m, n, k, a, n, t);
    lw = -1; LAPACK_dorglq(&M, &N, &K, b, &LDA, t, &q, &lw, &info); lw = (lapack_int)q; double *w = malloc(sizeof(double)*lw);
    LAPACK_dorglq(&M, &N, &K, b, &LDA, t, w, &lw, &info);
    rep("dorgqr Q", md_d(a, b, m*n), bw(a, b, sizeof(double)*m*n)); free(a); free(b); free(t); free(w); }
  { zc *a = malloc(sizeof(zc)*m*n), *b = malloc(sizeof(zc)*m*n), *t = malloc(sizeof(zc)*n), *tc = malloc(sizeof(zc)*n), q;
    fill_z(a, m*n); LAPACKE_zgeqrf(LAPACK_ROW_MAJOR, m, n, a, n, t); memcpy(b, a, sizeof(zc)*m*n);
    zc *b2 = malloc(sizeof(zc)*m*n); memcpy(b2, a, sizeof(zc)*m*n);
    LAPACKE_zungqr(LAPACK_ROW_MAJOR, m, n, k, a, n, t);
    lw = -1; LAPACK_zunglq(&M, &N, &K, b, &LDA, t, &q, &lw, &info); lw = (lapack_int)creal(q); zc *w = malloc(sizeof(zc)*lw);
    LAPACK_zunglq(&M, &N, &K, b, &LDA, t, w, &lw, &info);
    rep("zungqr Q, tau passed unchanged", md_z(a, b, m*n), 0);
    for (int i = 0; i < k; i++) tc[i] = conj(t[i]);
    LAPACK_zunglq(&M, &N, &K, b2, &LDA, tc, w, &lw, &info);
    rep("zungqr Q, conj(tau) passed", md_z(a, b2, m*n), bw(a, b2, sizeof(zc)*m*n));
    free(a); free(b); free(b2); free(t); free(tc); free(w); }
}

static void t_syevd(int n) {
  printf("syevd/heevd (n=%d)\n", n);
  for (int jv = 0; jv < 2; jv++) for (int u = 0; u < 2; u++) {
    char jobz = jv ? 'V' : 'N', uplo = u ? 'U' : 'L', fu = flip(uplo); lapack_int nn = n, info, lw = -1, liw = -1, qi;
    double *a = malloc(sizeof(double)*n*n), *b = malloc(sizeof(double)*n*n), *c = malloc(sizeof(double)*n*n), *w1 = malloc(sizeof(double)*n), *w2 = malloc(sizeof(double)*n), *w3 = malloc(sizeof(double)*n), q;
    fill_d(a, n*n); for (int i = 0; i < n; i++) for (int j = 0; j < i; j++) a[i*n+j] = a[j*n+i];
    memcpy(b, a, sizeof(double)*n*n); memcpy(c, a, sizeof(double)*n*n);
    LAPACKE_dsyevd(LAPACK_ROW_MAJOR, jobz, uplo, n, a, n, w1);
    LAPACK_dsyevd(&jobz, &fu, &nn, b, &nn, w2, &q, &lw, &qi, &liw, &info); lw = (lapack_int)q; liw = qi;
    double *wk = malloc(sizeof(double)*lw); lapack_int *iw = malloc(sizeof(lapack_int)*liw);
    LAPACK_dsyevd(&jobz, &fu, &nn, b, &nn, w2, wk, &lw, iw, &liw, &info);
    char nm[96]; snprintf(nm, 96, "dsyevd jobz=%c uplo=%c  [flip uplo] w", jobz, uplo); rep(nm, md_d(w1, w2, n), bw(w1, w2, sizeof(double)*n));
    if (jobz == 'V') { int nf; trd(n, b, n); double g = gauge_d(n, n, a, n, b, n, &nf);
      snprintf(nm, 96, "dsyevd jobz=V uplo=%c  [flip uplo + in-place T] Z mod sign (%d/%d flipped)", uplo, nf, n); rep(nm, g, bw(a, b, sizeof(double)*n*n)); }
    trd(n, c, n); LAPACK_dsyevd(&jobz, &uplo, &nn, c, &nn, w3, wk, &lw, iw, &liw, &info); trd(n, c, n);
    snprintf(nm, 96, "dsyevd jobz=%c uplo=%c  [in-place T, same uplo] w", jobz, uplo); rep(nm, md_d(w1, w3, n), bw(w1, w3, sizeof(double)*n));
    snprintf(nm, 96, "dsyevd jobz=%c uplo=%c  [in-place T, same uplo] A/Z", jobz, uplo); rep(nm, md_d(a, c, n*n), bw(a, c, sizeof(double)*n*n));
    free(a); free(b); free(c); free(w1); free(w2); free(w3); free(wk); free(iw); }
  for (int jv = 0; jv < 2; jv++) for (int u = 0; u < 2; u++) {
    char jobz = jv ? 'V' : 'N', uplo = u ? 'U' : 'L', fu = flip(uplo); lapack_int nn = n, info, lw = -1, lrw = -1, liw = -1, qi;
    zc *a = malloc(sizeof(zc)*n*n), *b = malloc(sizeof(zc)*n*n), *c = malloc(sizeof(zc)*n*n), q; double *w1 = malloc(sizeof(double)*n), *w2 = malloc(sizeof(double)*n), *w3 = malloc(sizeof(double)*n), qr;
    fill_z(a, n*n); for (int i = 0; i < n; i++) { a[i*n+i] = creal(a[i*n+i]); for (int j = 0; j < i; j++) a[i*n+j] = conj(a[j*n+i]); }
    memcpy(b, a, sizeof(zc)*n*n); memcpy(c, a, sizeof(zc)*n*n);
    LAPACKE_zheevd(LAPACK_ROW_MAJOR, jobz, uplo, n, a, n, w1);
    LAPACK_zheevd(&jobz, &fu, &nn, b, &nn, w2, &q, &lw, &qr, &lrw, &qi, &liw, &info); lw = (lapack_int)creal(q); lrw = (lapack_int)qr; liw = qi;
    zc *wk = malloc(sizeof(zc)*lw); double *rw = malloc(sizeof(double)*lrw); lapack_int *iw = malloc(sizeof(lapack_int)*liw);
    LAPACK_zheevd(&jobz, &fu, &nn, b, &nn, w2, wk, &lw, rw, &lrw, iw, &liw, &info);
    char nm[96]; snprintf(nm, 96, "zheevd jobz=%c uplo=%c  [flip uplo] w", jobz, uplo); rep(nm, md_d(w1, w2, n), bw(w1, w2, sizeof(double)*n));
    if (jobz == 'V') { int nf; trz(n, b, n, 1); double g = gauge_z(n, n, a, n, b, n, &nf);
      snprintf(nm, 96, "zheevd jobz=V uplo=%c  [flip + in-place conjT] Z mod phase (%d/%d differ)", uplo, nf, n); rep(nm, g, bw(a, b, sizeof(zc)*n*n)); }
    trz(n, c, n, 0); LAPACK_zheevd(&jobz, &uplo, &nn, c, &nn, w3, wk, &lw, rw, &lrw, iw, &liw, &info); trz(n, c, n, 0);
    snprintf(nm, 96, "zheevd jobz=%c uplo=%c  [in-place T, same uplo] w", jobz, uplo); rep(nm, md_d(w1, w3, n), bw(w1, w3, sizeof(double)*n));
    snprintf(nm, 96, "zheevd jobz=%c uplo=%c  [in-place T, same uplo] A/Z", jobz, uplo); rep(nm, md_z(a, c, n*n), bw(a, c, sizeof(zc)*n*n));
    free(a); free(b); free(c); free(w1); free(w2); free(w3); free(wk); free(rw); free(iw); }
}

/* gesdd: row-major (m x n) == col-major A^T (n x m); swap U<->VT roles. */
static void t_gesdd(int m, int n, char jobz) {
  int k = m < n ? m : n;
  int ucols = (jobz == 'A' || (jobz == 'O' && m < n)) ? m : (jobz == 'S' || jobz == 'O') ? k : 1;
  int vtrows = (jobz == 'A' || (jobz == 'O' && m >= n)) ? n : (jobz == 'S' || jobz == 'O') ? k : 1;
  int ldu = ucols, ldvt = n; int urows = (jobz == 'N' || (jobz == 'O' && m >= n)) ? 0 : m; if (jobz == 'O' && m >= n) urows = 0;
  double *a = malloc(sizeof(double)*m*n), *b = malloc(sizeof(double)*m*n), *s1 = malloc(sizeof(double)*k), *s2 = malloc(sizeof(double)*k);
  double *u1 = calloc((size_t)m*m+1, sizeof(double)), *u2 = calloc((size_t)m*m+1, sizeof(double)), *v1 = calloc((size_t)n*n+1, sizeof(double)), *v2 = calloc((size_t)n*n+1, sizeof(double)), q;
  fill_d(a, m*n); memcpy(b, a, sizeof(double)*m*n);
  LAPACKE_dgesdd(LAPACK_ROW_MAJOR, jobz, m, n, a, n, s1, u1, ldu, v1, ldvt);
  /* Fortran: M'=n, N'=m, A=b lda=n, U' -> caller vt (ldu'=ldvt), VT' -> caller u (ldvt'=ldu) */
  lapack_int M = n, N = m, LDA = n, LDU = ldvt, LDVT = ldu, info, lw = -1; lapack_int *iw = malloc(sizeof(lapack_int)*8*k);
  LAPACK_dgesdd(&jobz, &M, &N, b, &LDA, s2, v2, &LDU, u2, &LDVT, &q, &lw, iw, &info); lw = (lapack_int)q; double *wk = malloc(sizeof(double)*lw);
  LAPACK_dgesdd(&jobz, &M, &N, b, &LDA, s2, v2, &LDU, u2, &LDVT, wk, &lw, iw, &info);
  printf("dgesdd (m=%d n=%d jobz=%c): gesdd on buffer as n x m, U<->VT swapped\n", m, n, jobz);
  rep("s", md_d(s1, s2, k), bw(s1, s2, sizeof(double)*k));
  int nf; char nm[96];
  if (jobz == 'S' || jobz == 'A') {
    double g = gauge_d(m, k, u1, ldu, u2, ldu, &nf); snprintf(nm, 96, "U (first k cols) mod sign (%d/%d flipped)", nf, k); rep(nm, g, bw(u1, u2, sizeof(double)*m*ldu));
    g = gauge_rows_d(k, n, v1, ldvt, v2, ldvt, &nf); snprintf(nm, 96, "VT (first k rows) mod sign (%d/%d flipped)", nf, k); rep(nm, g, bw(v1, v2, sizeof(double)*vtrows*ldvt));
  } else if (jobz == 'O') {
    if (m >= n) { double g = gauge_d(m, k, a, n, b, n, &nf); snprintf(nm, 96, "A<-U (m>=n) mod sign (%d/%d flipped)", nf, k); rep(nm, g, 0);
      g = gauge_rows_d(k, n, v1, ldvt, v2, ldvt, &nf); snprintf(nm, 96, "VT mod sign (%d/%d flipped)", nf, k); rep(nm, g, 0); }
    else { double g = gauge_rows_d(k, n, a, n, b, n, &nf); snprintf(nm, 96, "A<-VT (m<n) mod sign (%d/%d flipped)", nf, k); rep(nm, g, 0);
      g = gauge_d(m, k, u1, ldu, u2, ldu, &nf); snprintf(nm, 96, "U mod sign (%d/%d flipped)", nf, k); rep(nm, g, 0); }
  }
  (void)urows; (void)vtrows;
  free(a); free(b); free(s1); free(s2); free(u1); free(u2); free(v1); free(v2); free(iw); free(wk);
}

static void t_zgesdd(int m, int n, char jobz) {
  int k = m < n ? m : n; int ldu = (jobz == 'A') ? m : k, ldvt = n;
  zc *a = malloc(sizeof(zc)*m*n), *b = malloc(sizeof(zc)*m*n), *u1 = calloc((size_t)m*m, sizeof(zc)), *u2 = calloc((size_t)m*m, sizeof(zc)), *v1 = calloc((size_t)n*n, sizeof(zc)), *v2 = calloc((size_t)n*n, sizeof(zc)), q;
  double *s1 = malloc(sizeof(double)*k), *s2 = malloc(sizeof(double)*k);
  fill_z(a, m*n); memcpy(b, a, sizeof(zc)*m*n);
  LAPACKE_zgesdd(LAPACK_ROW_MAJOR, jobz, m, n, a, n, s1, u1, ldu, v1, ldvt);
  lapack_int M = n, N = m, LDA = n, LDU = ldvt, LDVT = ldu, info, lw = -1; lapack_int *iw = malloc(sizeof(lapack_int)*8*k);
  int mx = m > n ? m : n; size_t lrw = (size_t)k * ((5*k + 7 > 2*mx + 2*k + 1) ? 5*k + 7 : 2*mx + 2*k + 1) + 10; double *rw = malloc(sizeof(double)*lrw);
  LAPACK_zgesdd(&jobz, &M, &N, b, &LDA, s2, v2, &LDU, u2, &LDVT, &q, &lw, rw, iw, &info); lw = (lapack_int)creal(q); zc *wk = malloc(sizeof(zc)*lw);
  LAPACK_zgesdd(&jobz, &M, &N, b, &LDA, s2, v2, &LDU, u2, &LDVT, wk, &lw, rw, iw, &info);
  printf("zgesdd (m=%d n=%d jobz=%c): U' <- conj? check\n", m, n, jobz);
  rep("s", md_d(s1, s2, k), bw(s1, s2, sizeof(double)*k));
  /* A = U S V^H. A^T = conj(V) S U^T  => Fortran U' = conj(V), VT' = U^T.
     Caller VT (= V^H) row-major == col-major V^H^T = conj(V) = U'  -> direct.
     Caller U row-major == col-major U^T = VT'                   -> direct.   */
  int nf; char nm[96]; double g = gauge_z(m, k, u1, ldu, u2, ldu, &nf); snprintf(nm, 96, "U cols mod phase (%d/%d differ)", nf, k); rep(nm, g, bw(u1, u2, sizeof(zc)*m*ldu));
  /* compare VT rows mod phase: transpose view -> conj */
  double mm = 0; nf = 0;
  for (int i = 0; i < k; i++) { zc dp = 0; for (int j = 0; j < n; j++) dp += conj(v2[i*ldvt+j]) * v1[i*ldvt+j]; zc ph = dp / cabs(dp); if (cabs(ph-1) > 1e-12) nf++;
    for (int j = 0; j < n; j++) mm = fmax(mm, cabs(v1[i*ldvt+j] - ph * v2[i*ldvt+j])); }
  snprintf(nm, 96, "VT rows mod phase (%d/%d differ)", nf, k); rep(nm, mm, 0);
  free(a); free(b); free(u1); free(u2); free(v1); free(v2); free(s1); free(s2); free(iw); free(rw); free(wk);
}

static void t_getrf(int n) {
  printf("getrf (n=%d)\n", n);
  double *a = malloc(sizeof(double)*n*n), *b = malloc(sizeof(double)*n*n), *c = malloc(sizeof(double)*n*n); lapack_int *p1 = malloc(sizeof(lapack_int)*n), *p2 = malloc(sizeof(lapack_int)*n), *p3 = malloc(sizeof(lapack_int)*n);
  fill_d(a, n*n); memcpy(b, a, sizeof(double)*n*n); memcpy(c, a, sizeof(double)*n*n);
  LAPACKE_dgetrf(LAPACK_ROW_MAJOR, n, n, a, n, p1);
  lapack_int nn = n, info; LAPACK_dgetrf(&nn, &nn, b, &nn, p2, &info);
  rep("dgetrf on buffer (=LU of A^T) vs netlib: factors", md_d(a, b, n*n), 0);
  printf("    ipiv equal: %s\n", memcmp(p1, p2, sizeof(lapack_int)*n) ? "no" : "YES");
  trd(n, c, n); LAPACK_dgetrf(&nn, &nn, c, &nn, p3, &info); trd(n, c, n);
  rep("dgetrf [in-place T] factors", md_d(a, c, n*n), bw(a, c, sizeof(double)*n*n)); printf("    ipiv equal: %s\n", memcmp(p1, p3, sizeof(lapack_int)*n) ? "no" : "YES");
  free(a); free(b); free(c); free(p1); free(p2); free(p3);
}

static void t_geev(int n) {
  printf("geev (n=%d): geev on buffer (=A^T)\n", n);
  double *a = malloc(sizeof(double)*n*n), *b = malloc(sizeof(double)*n*n), *wr1 = malloc(sizeof(double)*n), *wi1 = malloc(sizeof(double)*n), *wr2 = malloc(sizeof(double)*n), *wi2 = malloc(sizeof(double)*n), *vl = malloc(sizeof(double)*n*n), *vr = malloc(sizeof(double)*n*n), q;
  fill_d(a, n*n); memcpy(b, a, sizeof(double)*n*n);
  LAPACKE_dgeev(LAPACK_ROW_MAJOR, 'N', 'N', n, a, n, wr1, wi1, vl, n, vr, n);
  char jn = 'N'; lapack_int nn = n, one = 1, info, lw = -1; LAPACK_dgeev(&jn, &jn, &nn, b, &nn, wr2, wi2, vl, &one, vr, &one, &q, &lw, &info); lw = (lapack_int)q; double *wk = malloc(sizeof(double)*lw);
  LAPACK_dgeev(&jn, &jn, &nn, b, &nn, wr2, wi2, vl, &one, vr, &one, wk, &lw, &info);
  { /* in-place transpose path with right eigenvectors */
    double *c = malloc(sizeof(double)*n*n), *d = malloc(sizeof(double)*n*n), *vr1 = malloc(sizeof(double)*n*n), *vr3 = malloc(sizeof(double)*n*n), *wr3 = malloc(sizeof(double)*n), *wi3 = malloc(sizeof(double)*n), *wr4 = malloc(sizeof(double)*n), *wi4 = malloc(sizeof(double)*n);
    fill_d(c, n*n); memcpy(d, c, sizeof(double)*n*n); char jv = 'V'; lapack_int lw2 = -1; double q2;
    LAPACKE_dgeev(LAPACK_ROW_MAJOR, 'N', 'V', n, c, n, wr3, wi3, vl, n, vr1, n);
    LAPACK_dgeev(&jn, &jv, &nn, d, &nn, wr4, wi4, vl, &one, vr3, &nn, &q2, &lw2, &info); lw2 = (lapack_int)q2; double *wk2 = malloc(sizeof(double)*lw2);
    trd(n, d, n); LAPACK_dgeev(&jn, &jv, &nn, d, &nn, wr4, wi4, vl, &one, vr3, &nn, wk2, &lw2, &info); trd(n, d, n); trd(n, vr3, n);
    rep("dgeev [in-place T on A and VR] A,w,VR", fmax(md_d(vr1, vr3, n*n), md_d(wr3, wr4, n)), bw(vr1, vr3, sizeof(double)*n*n) && bw(wr3, wr4, sizeof(double)*n) && bw(wi3, wi4, sizeof(double)*n) && bw(c, d, sizeof(double)*n*n));
    free(c); free(d); free(vr1); free(vr3); free(wr3); free(wi3); free(wr4); free(wi4); free(wk2); }
  int same_order = 1; for (int i = 0; i < n; i++) if (fabs(wr1[i]-wr2[i]) + fabs(wi1[i]-wi2[i]) > 1e-8) same_order = 0;
  printf("    eigenvalues in same order as netlib: %s (first: netlib %.6f%+.6fi, trick %.6f%+.6fi)\n", same_order ? "YES" : "no", wr1[0], wi1[0], wr2[0], wi2[0]);
  free(a); free(b); free(wr1); free(wi1); free(wr2); free(wi2); free(vl); free(vr); free(wk);
}

static void t_sytrd(int n) {
  printf("sytrd (n=%d)\n", n);
  for (int u = 0; u < 2; u++) { char uplo = u ? 'U' : 'L', fu = flip(uplo);
    double *a = malloc(sizeof(double)*n*n), *b = malloc(sizeof(double)*n*n), *c = malloc(sizeof(double)*n*n), *d1 = malloc(sizeof(double)*n), *d2 = malloc(sizeof(double)*n), *d3 = malloc(sizeof(double)*n), *e1 = malloc(sizeof(double)*n), *e2 = malloc(sizeof(double)*n), *e3 = malloc(sizeof(double)*n), *t1 = malloc(sizeof(double)*n), *t2 = malloc(sizeof(double)*n), *t3 = malloc(sizeof(double)*n), q;
    fill_d(a, n*n); for (int i = 0; i < n; i++) for (int j = 0; j < i; j++) a[i*n+j] = a[j*n+i];
    memcpy(b, a, sizeof(double)*n*n); memcpy(c, a, sizeof(double)*n*n);
    LAPACKE_dsytrd(LAPACK_ROW_MAJOR, uplo, n, a, n, d1, e1, t1);
    lapack_int nn = n, info, lw = -1; LAPACK_dsytrd(&fu, &nn, b, &nn, d2, e2, t2, &q, &lw, &info); lw = (lapack_int)q; double *wk = malloc(sizeof(double)*lw);
    LAPACK_dsytrd(&fu, &nn, b, &nn, d2, e2, t2, wk, &lw, &info);
    char nm[96]; snprintf(nm, 96, "dsytrd uplo=%c [flip uplo] d", uplo); rep(nm, md_d(d1, d2, n), 0);
    trd(n, c, n); LAPACK_dsytrd(&uplo, &nn, c, &nn, d3, e3, t3, wk, &lw, &info); trd(n, c, n);
    snprintf(nm, 96, "dsytrd uplo=%c [in-place T] A,d,e,tau", uplo);
    int ok = bw(a, c, sizeof(double)*n*n) && bw(d1, d3, sizeof(double)*n) && bw(e1, e3, sizeof(double)*(n-1)) && bw(t1, t3, sizeof(double)*(n-1));
    rep(nm, fmax(md_d(a, c, n*n), md_d(d1, d3, n)), ok);
    free(a); free(b); free(c); free(d1); free(d2); free(d3); free(e1); free(e2); free(e3); free(t1); free(t2); free(t3); free(wk); }
}

static void t_gehrd_gees(int n) {
  printf("gehrd / gees (n=%d): in-place transpose path\n", n);
  double *a = malloc(sizeof(double)*n*n), *c = malloc(sizeof(double)*n*n), *t1 = malloc(sizeof(double)*n), *t3 = malloc(sizeof(double)*n), q;
  fill_d(a, n*n); memcpy(c, a, sizeof(double)*n*n);
  LAPACKE_dgehrd(LAPACK_ROW_MAJOR, n, 1, n, a, n, t1);
  lapack_int nn = n, ilo = 1, info, lw = -1; LAPACK_dgehrd(&nn, &ilo, &nn, c, &nn, t3, &q, &lw, &info); lw = (lapack_int)q; double *wk = malloc(sizeof(double)*lw);
  trd(n, c, n); LAPACK_dgehrd(&nn, &ilo, &nn, c, &nn, t3, wk, &lw, &info); trd(n, c, n);
  rep("dgehrd [in-place T] A,tau", fmax(md_d(a, c, n*n), md_d(t1, t3, n-1)), bw(a, c, sizeof(double)*n*n) && bw(t1, t3, sizeof(double)*(n-1)));
  double *g = malloc(sizeof(double)*n*n), *h = malloc(sizeof(double)*n*n), *vs1 = malloc(sizeof(double)*n*n), *vs3 = malloc(sizeof(double)*n*n), *wr1 = malloc(sizeof(double)*n), *wi1 = malloc(sizeof(double)*n), *wr3 = malloc(sizeof(double)*n), *wi3 = malloc(sizeof(double)*n);
  fill_d(g, n*n); memcpy(h, g, sizeof(double)*n*n); lapack_int sd1, sd3; lapack_logical bwk[1];
  LAPACKE_dgees(LAPACK_ROW_MAJOR, 'V', 'N', NULL, n, g, n, &sd1, wr1, wi1, vs1, n);
  char jv = 'V', sn = 'N'; lw = -1; LAPACK_dgees(&jv, &sn, NULL, &nn, h, &nn, &sd3, wr3, wi3, vs3, &nn, &q, &lw, bwk, &info); lw = (lapack_int)q; double *wk2 = malloc(sizeof(double)*lw);
  trd(n, h, n); LAPACK_dgees(&jv, &sn, NULL, &nn, h, &nn, &sd3, wr3, wi3, vs3, &nn, wk2, &lw, bwk, &info); trd(n, h, n); trd(n, vs3, n);
  rep("dgees [in-place T on A and VS] T,VS,w", fmax(md_d(g, h, n*n), md_d(vs1, vs3, n*n)), bw(g, h, sizeof(double)*n*n) && bw(vs1, vs3, sizeof(double)*n*n) && bw(wr1, wr3, sizeof(double)*n));
  free(a); free(c); free(t1); free(t3); free(wk); free(g); free(h); free(vs1); free(vs3); free(wr1); free(wi1); free(wr3); free(wi3); free(wk2);
}

/* Solvers (not used by Catalyst): getrs / potrs / trtrs with row-major multi-RHS B
   via right-side trsm on B^T (= row-major B buffer), no copies. */
static void t_solvers(int n, int nrhs) {
  printf("getrs/potrs/trtrs (n=%d nrhs=%d): right-side Fortran ?trsm on row-major B buffer\n", n, nrhs);
  double *a = malloc(sizeof(double)*n*n), *b1 = malloc(sizeof(double)*n*nrhs), *b2 = malloc(sizeof(double)*n*nrhs); lapack_int *ip = malloc(sizeof(lapack_int)*n);
  fill_d(a, n*n); fill_d(b1, n*nrhs); memcpy(b2, b1, sizeof(double)*n*nrhs);
  LAPACKE_dgetrf(LAPACK_ROW_MAJOR, n, n, a, n, ip);
  for (int t = 0; t < 2; t++) { char tr = t ? 'T' : 'N';
    fill_d(b1, n*nrhs); memcpy(b2, b1, sizeof(double)*n*nrhs);
    LAPACKE_dgetrs(LAPACK_ROW_MAJOR, tr, n, nrhs, a, n, ip, b1, nrhs);
    /* buffer of factors seen col-major = F^T: strict upper holds L^T (unit), lower holds U^T.
       B row-major n x nrhs == Bc = B^T col-major nrhs x n (ld nrhs).  Solve op(A) X = B  <=>  Xc op(A)^T = Bc. */
    lapack_int R = nrhs, N = n; double one = 1.0; char Rs = 'R', Up = 'U', Lo = 'L', No = 'N', Tr = 'T', Un = 'U', Nu = 'N';
    if (tr == 'N') { /* A = P L U; X = U^-1 L^-1 P^T B */
      for (int i = 0; i < n; i++) if (ip[i]-1 != i) cblas_dswap(nrhs, b2 + i*nrhs, 1, b2 + (ip[i]-1)*nrhs, 1);
      LAPACK_dtrsm_base(&Rs, &Up, &No, &Un, &R, &N, &one, a, &N, b2, &R, 1, 1, 1, 1);   /* Y L^T... Xc (L^T) = Bc, L^T in upper of M, unit */
      LAPACK_dtrsm_base(&Rs, &Lo, &No, &Nu, &R, &N, &one, a, &N, b2, &R, 1, 1, 1, 1);   /* Xc (U^T) = Yc, U^T in lower of M */
    } else { /* A^T X = B: X = P L^-T U^-T B -> Xc = Bc U^-1 L^-1 then column perms reversed */
      LAPACK_dtrsm_base(&Rs, &Lo, &Tr, &Nu, &R, &N, &one, a, &N, b2, &R, 1, 1, 1, 1);
      LAPACK_dtrsm_base(&Rs, &Up, &Tr, &Un, &R, &N, &one, a, &N, b2, &R, 1, 1, 1, 1);
      for (int i = n-1; i >= 0; i--) if (ip[i]-1 != i) cblas_dswap(nrhs, b2 + i*nrhs, 1, b2 + (ip[i]-1)*nrhs, 1);
    }
    char nm[64]; snprintf(nm, 64, "dgetrs trans=%c X", tr); rep(nm, md_d(b1, b2, n*nrhs), bw(b1, b2, sizeof(double)*n*nrhs)); }
  /* ztrtrs trans='C' — not expressible as a left-side col-major trtrs on the buffer */
  zc *z = malloc(sizeof(zc)*n*n), *zb1 = malloc(sizeof(zc)*n*nrhs), *zb2 = malloc(sizeof(zc)*n*nrhs); fill_z(z, n*n); for (int i = 0; i < n; i++) z[i*n+i] += n;
  fill_z(zb1, n*nrhs); memcpy(zb2, zb1, sizeof(zc)*n*nrhs);
  LAPACKE_ztrtrs(LAPACK_ROW_MAJOR, 'L', 'C', 'N', n, nrhs, z, n, zb1, nrhs);
  { char Rs = 'R', Up = 'U', Cj = 'C', Nu = 'N'; lapack_int R = nrhs, N = n; zc one = 1.0;
    /* op(A)=A^H, A lower row-major; M = buffer col-major = A^T (upper). Xc (A^H)^T = Bc, (A^H)^T = conj(A) = M^H */
    LAPACK_ztrsm_base(&Rs, &Up, &Cj, &Nu, &R, &N, &one, z, &N, zb2, &R, 1, 1, 1, 1); }
  rep("ztrtrs uplo=L trans=C X (right-side ztrsm, uplo flipped, 'C')", md_z(zb1, zb2, n*nrhs), bw(zb1, zb2, sizeof(zc)*n*nrhs));
  free(a); free(b1); free(b2); free(ip); free(z); free(zb1); free(zb2);
}

/* gesdd jobz='O' with m == n: the swap puts VT into A and U into the vt buffer. */
static void t_gesdd_O_square(int n) {
  printf("dgesdd (m=n=%d jobz=O): swap trick role check\n", n);
  double *a = malloc(sizeof(double)*n*n), *b = malloc(sizeof(double)*n*n), *s1 = malloc(sizeof(double)*n), *s2 = malloc(sizeof(double)*n), *v1 = malloc(sizeof(double)*n*n), *v2 = malloc(sizeof(double)*n*n), du, q;
  fill_d(a, n*n); memcpy(b, a, sizeof(double)*n*n);
  LAPACKE_dgesdd(LAPACK_ROW_MAJOR, 'O', n, n, a, n, s1, &du, 1, v1, n);
  char jz = 'O'; lapack_int N = n, one = 1, info, lw = -1; lapack_int *iw = malloc(sizeof(lapack_int)*8*n);
  LAPACK_dgesdd(&jz, &N, &N, b, &N, s2, &du, &one, v2, &N, &q, &lw, iw, &info); lw = (lapack_int)q; double *wk = malloc(sizeof(double)*lw);
  LAPACK_dgesdd(&jz, &N, &N, b, &N, s2, &du, &one, v2, &N, wk, &lw, iw, &info);
  int nf; double g1 = gauge_d(n, n, a, n, b, n, &nf), g2 = gauge_d(n, n, a, n, v2, n, &nf), g3 = gauge_rows_d(n, n, v1, n, b, n, &nf);
  printf("    netlib A(=U) vs trick A, mod sign: %.2e | netlib A(=U) vs trick vt-buffer: %.2e | netlib vt vs trick A (rows): %.2e\n", g1, g2, g3);
  free(a); free(b); free(s1); free(s2); free(v1); free(v2); free(iw); free(wk);
}

/* in-place transpose with lda > n (square). */
static void t_getrf_lda(int n, int lda) {
  printf("getrf in-place transpose with lda=%d > n=%d\n", lda, n);
  double *a = malloc(sizeof(double)*n*lda), *c = malloc(sizeof(double)*n*lda); lapack_int *p1 = malloc(sizeof(lapack_int)*n), *p3 = malloc(sizeof(lapack_int)*n);
  fill_d(a, n*lda); memcpy(c, a, sizeof(double)*n*lda);
  LAPACKE_dgetrf(LAPACK_ROW_MAJOR, n, n, a, lda, p1);
  lapack_int nn = n, LD = lda, info; trd(n, c, lda); LAPACK_dgetrf(&nn, &nn, c, &LD, p3, &info); trd(n, c, lda);
  rep("dgetrf [in-place T, lda>n] whole buffer incl. padding", md_d(a, c, n*lda), bw(a, c, sizeof(double)*n*lda));
  free(a); free(c); free(p1); free(p3);
}

int main(int argc, char **argv) {
  int n = argc > 1 ? atoi(argv[1]) : 200;
  printf("backend: %s\n", openblas_get_config());
  t_gesdd_O_square(n); t_getrf_lda(n, n + 5);
  t_potrf(n);
  t_geqrf(n + 37, n); t_geqrf(n, n + 37); t_geqrf(n, n);
  t_orgqr(n + 37, n, n); t_orgqr(n + 37, n, n - 13);
  t_syevd(n);
  t_gesdd(n + 37, n, 'S'); t_gesdd(n, n + 37, 'S'); t_gesdd(n, n, 'A'); t_gesdd(n + 37, n, 'N');
  t_gesdd(n + 37, n, 'O'); t_gesdd(n, n + 37, 'O');
  t_zgesdd(n + 37, n, 'S'); t_zgesdd(n, n + 37, 'A');
  t_getrf(n); t_geev(n); t_sytrd(n); t_gehrd_gees(n);
  t_solvers(n, 7);
  return 0;
}
