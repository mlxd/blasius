/* Both renamed OpenBLAS widths in one binary: bl_lp_* (int) and bl_ilp_* (int64_t). */
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
double bl_lp_cblas_ddot(int, const double*, int, const double*, int);
double bl_ilp_cblas_ddot(int64_t, const double*, int64_t, const double*, int64_t);
void bl_lp_cblas_dgemm(int, int, int, int, int, int, double, const double*, int, const double*, int, double, double*, int);
void bl_ilp_cblas_dgemm(int, int, int, int64_t, int64_t, int64_t, double, const double*, int64_t, const double*, int64_t, double, double*, int64_t);
int bl_lp_LAPACKE_dgesv(int, int, int, double*, int, int*, double*, int);
int64_t bl_ilp_LAPACKE_dgesv(int, int64_t, int64_t, double*, int64_t, int64_t*, double*, int64_t);
static int N = 300;
static double check(int ilp) {
    int n = N; double* a = malloc(8 * n * n), *c = malloc(8 * n * n), *b = malloc(8 * n), *x = malloc(8 * n), worst = 0;
    for (int i = 0; i < n * n; i++) a[i] = ((i * 7919) % 1000) / 1000.0 - 0.5;
    for (int i = 0; i < n; i++) { a[i * n + i] += n; b[i] = i + 1; x[i] = b[i]; }
    double d = ilp ? bl_ilp_cblas_ddot(n, b, 1, b, 1) : bl_lp_cblas_ddot(n, b, 1, b, 1), e = 0;
    for (int i = 0; i < n; i++) e += b[i] * b[i];
    worst = fabs(d - e) / e;
    if (ilp) bl_ilp_cblas_dgemm(102, 111, 111, n, n, n, 1, a, n, a, n, 0, c, n); else bl_lp_cblas_dgemm(102, 111, 111, n, n, n, 1, a, n, a, n, 0, c, n);
    for (int j = 0; j < 20; j++) for (int i = 0; i < 20; i++) { double s = 0; for (int k = 0; k < n; k++) s += a[i + k * n] * a[k + j * n]; worst = fmax(worst, fabs(s - c[i + j * n]) / n); }
    double* f = malloc(8 * n * n); for (int i = 0; i < n * n; i++) f[i] = a[i];
    long info;
    if (ilp) { int64_t* p = malloc(8 * n); info = bl_ilp_LAPACKE_dgesv(102, n, 1, f, n, p, x, n); free(p); }
    else { int* p = malloc(4 * n); info = bl_lp_LAPACKE_dgesv(102, n, 1, f, n, p, x, n); free(p); }
    for (int i = 0; i < n; i++) { double s = 0; for (int k = 0; k < n; k++) s += a[i + k * n] * x[k]; worst = fmax(worst, fabs(s - b[i]) / n); }
    free(a); free(c); free(b); free(x); free(f);
    return info ? 1e30 : worst;
}
static void* th(void* w) { double* r = malloc(8); *r = 0; for (int k = 0; k < 5; k++) *r = fmax(*r, check((int)(long)w)); return r; }
int main(void) {
    double lp = check(0), ilp = check(1);
    printf("sequential: lp64 %s (%.1e)  ilp64 %s (%.1e)\n", lp < 1e-9 ? "CORRECT" : "WRONG", lp, ilp < 1e-9 ? "CORRECT" : "WRONG", ilp);
    pthread_t t[4]; for (long i = 0; i < 4; i++) pthread_create(&t[i], 0, th, (void*)(i & 1));
    double w = 0; for (int i = 0; i < 4; i++) { double* r; pthread_join(t[i], (void**)&r); w = fmax(w, *r); free(r); }
    printf("4 threads, both widths concurrently: %s (%.1e)\n", w < 1e-9 ? "CORRECT" : "WRONG", w);
    return !(lp < 1e-9 && ilp < 1e-9 && w < 1e-9);
}
