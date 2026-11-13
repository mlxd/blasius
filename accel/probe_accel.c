/* Accelerate provider facts, items (1)-(3) of blasius_pvt#20, plus the LP64
 * cblas_i?amax return width.
 *
 *   (1) dlsym of $NEWLAPACK / $NEWLAPACK$ILP64 names from a dlopen of the
 *       framework path, and a correctness call through each pointer.
 *   (2) any LAPACKE_* names reachable by dlsym (dyld_info covers the export list).
 *   (3) whether BLASSetThreading(SINGLE) affects each family, judged by the
 *       CPU-time / wall-time ratio of a large dgemm and dgetrf.
 *
 * Build: clang -O2 -o probe_accel probe_accel.c   (no -framework: everything via dlopen)
 */
#include <dlfcn.h>
#include <mach/mach.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

#define ACC "/System/Library/Frameworks/Accelerate.framework/Accelerate"
static void* H;

static double now(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec + 1e-9 * t.tv_nsec; }
static double cpu(void) { struct rusage r; getrusage(RUSAGE_SELF, &r); return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec); }
static int nthreads(void) {
    thread_act_array_t l; mach_msg_type_number_t c = 0;
    if (task_threads(mach_task_self(), &l, &c) != KERN_SUCCESS) return -1;
    for (mach_msg_type_number_t i = 0; i < c; i++) mach_port_deallocate(mach_task_self(), l[i]);
    vm_deallocate(mach_task_self(), (vm_address_t)l, c * sizeof(thread_act_t));
    return (int)c;
}

typedef void (*dgemm32_t)(int, int, int, int, int, int, double, const double*, int, const double*, int, double, double*, int);
typedef void (*dgemm64_t)(int, int, int, long, long, long, double, const double*, long, const double*, long, double, double*, long);
/* Fortran-ABI dgetrf / dgesv: legacy and LP64 take int*, ILP64 takes long* */
typedef void (*getrf32_t)(const int*, const int*, double*, const int*, int*, int*);
typedef void (*getrf64_t)(const long*, const long*, double*, const long*, long*, long*);
typedef void (*gesv32_t)(const int*, const int*, double*, const int*, int*, double*, const int*, int*);
typedef void (*gesv64_t)(const long*, const long*, double*, const long*, long*, double*, const long*, long*);

struct fam { const char* name; const char* sfx; int ilp; const char* getrf; const char* gesv; };
static const struct fam F[] = {
    {"legacy", "", 0, "dgetrf_", "dgesv_"},
    {"new-lp64", "$NEWLAPACK", 0, "dgetrf$NEWLAPACK", "dgesv$NEWLAPACK"},
    {"new-ilp64", "$NEWLAPACK$ILP64", 1, "dgetrf$NEWLAPACK$ILP64", "dgesv$NEWLAPACK$ILP64"},
};

static void* S(const char* base, const char* sfx) { char n[128]; snprintf(n, sizeof n, "%s%s", base, sfx); return dlsym(H, n); }

static void item1(void) {
    printf("== (1) dlsym from dlopen(\"%s\")\n", ACC);
    for (int f = 0; f < 3; f++) {
        void* g = S("cblas_dgemm", F[f].sfx); void* sv = dlsym(H, F[f].gesv);
        printf("  %-10s cblas_dgemm%-18s %s   %-24s %s\n", F[f].name, F[f].sfx, g ? "FOUND" : "missing", F[f].gesv, sv ? "FOUND" : "missing");
        if (!g || !sv) continue;
        /* 2x2 dgemm: [1 2;3 4]*[5 6;7 8] = [19 22;43 50] (row-major) */
        double a[4] = {1, 2, 3, 4}, b[4] = {5, 6, 7, 8}, c[4] = {0};
        if (F[f].ilp) ((dgemm64_t)g)(101, 111, 111, 2, 2, 2, 1, a, 2, b, 2, 0, c, 2);
        else ((dgemm32_t)g)(101, 111, 111, 2, 2, 2, 1, a, 2, b, 2, 0, c, 2);
        int okg = c[0] == 19 && c[1] == 22 && c[2] == 43 && c[3] == 50;
        /* dgesv: [4 1;2 3] x = [1;2] -> x = [0.1, 0.6] (col-major) */
        double A[4] = {4, 2, 1, 3}, B[2] = {1, 2}; int oks;
        if (F[f].ilp) { long n = 2, one = 1, piv[2], info = -9; ((gesv64_t)sv)(&n, &one, A, &n, piv, B, &n, &info); oks = info == 0; }
        else { int n = 2, one = 1, piv[2], info = -9; ((gesv32_t)sv)(&n, &one, A, &n, piv, B, &n, &info); oks = info == 0; }
        oks = oks && B[0] > 0.0999999 && B[0] < 0.1000001 && B[1] > 0.5999999 && B[1] < 0.6000001;
        printf("             dgemm result %s, dgesv result %s\n", okg ? "CORRECT" : "WRONG", oks ? "CORRECT" : "WRONG");
    }
}

static void item2(void) {
    static const char* names[] = {"LAPACKE_dgesv", "LAPACKE_dgesv_work", "LAPACKE_dgesv$NEWLAPACK", "LAPACKE_dgesv$NEWLAPACK$ILP64",
                                  "LAPACKE_dgeqrf", "LAPACKE_zheev", "LAPACKE_get_nancheck", "LAPACKE_xerbla", 0};
    printf("== (2) LAPACKE names via dlsym (Accelerate handle, then RTLD_DEFAULT)\n");
    int any = 0;
    for (int i = 0; names[i]; i++) {
        void* p = dlsym(H, names[i]); void* q = dlsym(RTLD_DEFAULT, names[i]);
        if (p || q) any = 1;
        printf("  %-32s handle=%s default=%s\n", names[i], p ? "FOUND" : "missing", q ? "FOUND" : "missing");
    }
    printf("  any LAPACKE symbol found: %s\n", any ? "YES" : "no");
}

/* one timed block: CPU/wall ratio > ~1.3 means more than one thread did work */
static void timed(const struct fam* f, const char* label, int n) {
    double* a = malloc(sizeof(double) * n * n), *b = malloc(sizeof(double) * n * n), *c = malloc(sizeof(double) * n * n);
    for (long i = 0; i < (long)n * n; i++) { a[i] = (double)((i * 7919) % 1000) / 1000.0 - 0.5; b[i] = a[i]; }
    for (long i = 0; i < n; i++) b[i * n + i] += n;
    void* g = S("cblas_dgemm", f->sfx); void* r = dlsym(H, f->getrf);
    double w0 = now(), c0 = cpu();
    for (int k = 0; k < 3; k++) {
        if (f->ilp) ((dgemm64_t)g)(102, 111, 111, n, n, n, 1, a, n, a, n, 0, c, n);
        else ((dgemm32_t)g)(102, 111, 111, n, n, n, 1, a, n, a, n, 0, c, n);
    }
    double w1 = now(), c1 = cpu();
    if (f->ilp) { long N = n, info; long* p = malloc(sizeof(long) * n); ((getrf64_t)r)(&N, &N, b, &N, p, &info); free(p); }
    else { int N = n, info; int* p = malloc(sizeof(int) * n); ((getrf32_t)r)(&N, &N, b, &N, p, &info); free(p); }
    double w2 = now(), c2 = cpu();
    printf("  %-10s %-8s dgemm cpu/wall=%5.2f (wall %.3fs)   dgetrf cpu/wall=%5.2f (wall %.3fs)   threads-in-task=%d\n", f->name, label,
           (c1 - c0) / (w1 - w0), w1 - w0, (c2 - c1) / (w2 - w1), w2 - w1, nthreads());
    free(a); free(b); free(c);
}

static int (*set_thr)(unsigned); static unsigned (*get_thr)(void);
static void* other_thread(void* x) { (void)x; printf("  new pthread sees BLASGetThreading()=%u (0=MULTI, 1=SINGLE)\n", get_thr()); return 0; }

static void item3(int n) {
    set_thr = (int (*)(unsigned))dlsym(H, "BLASSetThreading");
    get_thr = (unsigned (*)(void))dlsym(H, "BLASGetThreading");
    printf("== (3) BLASSetThreading per family (n=%d). VECLIB_MAXIMUM_THREADS=%s\n", n, getenv("VECLIB_MAXIMUM_THREADS") ? getenv("VECLIB_MAXIMUM_THREADS") : "(unset)");
    printf("  BLASSetThreading %s, BLASGetThreading %s; threads-in-task at start=%d\n", set_thr ? "FOUND" : "missing", get_thr ? "FOUND" : "missing", nthreads());
    if (!set_thr) { for (int f = 0; f < 3; f++) timed(&F[f], "default", n); return; }
    printf("  initial BLASGetThreading()=%u\n", get_thr());
    int rc = set_thr(1);
    printf("  BLASSetThreading(SINGLE)=%d, now %u\n", rc, get_thr());
    pthread_t t; pthread_create(&t, 0, other_thread, 0); pthread_join(t, 0);
    for (int f = 0; f < 3; f++) timed(&F[f], "SINGLE", n);
    rc = set_thr(0);
    printf("  BLASSetThreading(MULTI)=%d, now %u\n", rc, get_thr());
    for (int f = 0; f < 3; f++) timed(&F[f], "MULTI", n);
    set_thr(1);
    for (int f = 0; f < 3; f++) timed(&F[f], "SINGLE2", n);
}

/* LP64 cblas_idamax returns int (Accelerate new API); read the full x0 register */
static void amax(void) {
    printf("== cblas_i?amax return register (upper 32 bits should be ignored for LP64)\n");
    double x[5] = {1, -9, 3, 2, 0};
    for (int f = 0; f < 3; f++) {
        void* p = S("cblas_idamax", F[f].sfx); if (!p) { printf("  %-10s missing\n", F[f].name); continue; }
        uint64_t r = F[f].ilp ? ((uint64_t (*)(long, const double*, long))p)(5, x, 1) : ((uint64_t (*)(int, const double*, int))p)(5, x, 1);
        printf("  %-10s raw x0=0x%016llx (expect index 1)%s\n", F[f].name, (unsigned long long)r, (r >> 32) ? "  UPPER BITS SET" : "");
    }
}

int main(int c, char** v) {
    H = dlopen(ACC, RTLD_LAZY | RTLD_LOCAL);
    if (!H) { printf("dlopen failed: %s\n", dlerror()); return 1; }
    item1(); item2(); amax();
    item3(c > 1 ? atoi(v[1]) : 2000);
    return 0;
}
