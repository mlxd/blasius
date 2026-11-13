/* ArmPL on macOS: does libarmpl_*_mp load and run by absolute path, and which libomp
 * does it get?   armpl_probe <libarmpl.dylib> [libomp.dylib to preload RTLD_GLOBAL]   */
#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>
static double now(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec + 1e-9 * t.tv_nsec; }
static double cpu(void) { struct rusage r; getrusage(RUSAGE_SELF, &r); return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec); }
typedef void (*dgemm_t)(int, int, int, int, int, int, double, const double*, int, const double*, int, double, double*, int);
typedef int (*gesv_t)(int, int, int, double*, int, int*, double*, int);
int main(int c, char** v) {
    if (c > 2) { void* o = dlopen(v[2], RTLD_NOW | RTLD_GLOBAL); printf("  preload %s: %s\n", v[2], o ? "ok" : dlerror()); }
    void* h = dlopen(v[1], RTLD_NOW | RTLD_LOCAL);
    if (!h) { printf("  dlopen FAILED: %s\n", dlerror()); return 1; }
    printf("  dlopen ok\n");
    for (uint32_t i = 0; i < _dyld_image_count(); i++) { const char* n = _dyld_get_image_name(i); if (strstr(n, "omp") || strstr(n, "armpl")) printf("  image: %s\n", n); }
    dgemm_t g = (dgemm_t)dlsym(h, "cblas_dgemm"); gesv_t s = (gesv_t)dlsym(h, "LAPACKE_dgesv");
    int (*mt)(void) = (int (*)(void))dlsym(RTLD_DEFAULT, "omp_get_max_threads");
    printf("  cblas_dgemm %s, LAPACKE_dgesv %s, omp_get_max_threads=%d\n", g ? "found" : "MISSING", s ? "found" : "MISSING", mt ? mt() : -1);
    if (!g || !s) return 1;
    double a[4] = {1, 2, 3, 4}, b[4] = {5, 6, 7, 8}, r[4]; g(101, 111, 111, 2, 2, 2, 1, a, 2, b, 2, 0, r, 2);
    double A[4] = {4, 1, 2, 3}, B[2] = {1, 2}; int p[2]; int info = s(101, 2, 1, A, 2, p, B, 1);
    printf("  dgemm %s, dgesv %s\n", r[0] == 19 && r[3] == 50 ? "CORRECT" : "WRONG", info == 0 && B[0] > 0.0999 && B[0] < 0.1001 ? "CORRECT" : "WRONG");
    int n = 2000; double* x = malloc(8.0 * n * n), *y = malloc(8.0 * n * n);
    for (long i = 0; i < (long)n * n; i++) x[i] = (i % 97) / 97.0;
    double w0 = now(), c0 = cpu(); for (int k = 0; k < 3; k++) g(102, 111, 111, n, n, n, 1, x, n, x, n, 0, y, n);
    printf("  dgemm n=%d cpu/wall=%.2f (wall %.3fs)\n", n, (cpu() - c0) / (now() - w0), now() - w0);
    return 0;
}
