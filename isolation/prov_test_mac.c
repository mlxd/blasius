/* macOS port of the provider-isolation harness (research/provider-isolation prov_test.c),
 * without the ELF rebinding: macOS relies on two-level namespaces.
 *
 *   prov_test_mac --slot 'path|prefix|suffix|ilp' [--slot ...] [--global lib ...]
 *                 [--swap] [--n N] [--iters K] [--callers C]
 *
 * Optionally dlopens --global libraries RTLD_GLOBAL, loads each slot RTLD_NOW | RTLD_LOCAL,
 * runs verified CBLAS/LAPACKE work (work.inc) from concurrent callers, and prints
 * PASS/WRONG. Any stub or intruder name that captures a provider call exits 42
 * after printing "INTRUDED <name>".
 */
#include <complex.h>
#include <dlfcn.h>
#include <getopt.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct { const char* lib; const char* pfx; const char* sfx; int ilp; void* h; } slot_t;
static slot_t slots[8]; static int nslots;
static __thread slot_t* cur; static int hits;
static void* sym(const char* b) { char n[256]; snprintf(n, sizeof n, "%s%s%s", cur->pfx, b, cur->sfx); return dlsym(cur->h, n); }
#define IDX int
#define SUF _lp
#include "work.inc"
#undef IDX
#undef SUF
#define IDX int64_t
#define SUF _ilp
#include "work.inc"
#undef IDX
#undef SUF
static int n = 300, iters = 3, callers = 4, swapping;
static double results[64];
static void* caller(void* a) {
    long id = (long)a; double w = 0;
    for (int i = 0; i < iters; i++) {
        int s = (swapping ? id + i : id) % nslots;
        cur = &slots[s];
        double e = cur->ilp ? run_all_ilp(id * 97 + i + 1, n) : run_all_lp(id * 97 + i + 1, n);
        if (e > w) w = e;
    }
    results[id] = w; return 0;
}
static char* field(char* s, char** rest) { char* b = strchr(s, '|'); if (b) { *b = 0; *rest = b + 1; } else *rest = s + strlen(s); return s; }
int main(int c, char** v) {
    const char* globals[8]; int ng = 0;
    static struct option o[] = {{"slot",1,0,'S'},{"global",1,0,'g'},{"n",1,0,'n'},{"iters",1,0,'t'},{"callers",1,0,'c'},{"swap",0,0,'w'},{0,0,0,0}};
    for (int x; (x = getopt_long(c, v, "", o, 0)) != -1;) switch (x) {
        case 'S': { char* r, *s = strdup(optarg); slot_t* t = &slots[nslots++]; t->lib = field(s, &r); t->pfx = field(r, &r); t->sfx = field(r, &r); t->ilp = r[0] == '1'; break; }
        case 'g': globals[ng++] = optarg; break; case 'n': n = atoi(optarg); break; case 't': iters = atoi(optarg); break;
        case 'c': callers = atoi(optarg); break; case 'w': swapping = 1; break; }
    for (int i = 0; i < ng; i++) if (!dlopen(globals[i], RTLD_NOW | RTLD_GLOBAL)) { printf("GLOBALFAIL %s\n", dlerror()); return 2; }
    for (int k = 0; k < nslots; k++) {
        slots[k].h = dlopen(slots[k].lib, RTLD_NOW | RTLD_LOCAL);
        if (!slots[k].h) { printf("LOADFAIL %s\n", dlerror()); return 3; }
    }
    pthread_t th[64]; pthread_attr_t at; pthread_attr_init(&at);
    if (!getenv("SMALL_STACK")) pthread_attr_setstacksize(&at, 16u << 20); /* macOS default is 512 KiB */
    for (long i = 0; i < callers; i++) pthread_create(&th[i], &at, caller, (void*)i);
    for (int i = 0; i < callers; i++) pthread_join(th[i], 0);
    double worst = 0; for (int i = 0; i < callers; i++) if (results[i] > worst) worst = results[i];
    int ok = worst < 1e-6 && hits > 0;
    printf("%s worst=%.1e calls=%d\n", ok ? "PASS" : "WRONG", worst, hits);
    fflush(stdout);
    return ok ? 0 : 1;
}
