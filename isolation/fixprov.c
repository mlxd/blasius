/* Fixture provider: an exported LAPACKE_dgeqrf that calls its own exported
 * LAPACKE_dgeqrf_work, and an internal routine calling its own exported dlarf1f_.
 * FIX_WEAK makes the callees weak definitions (sets MH_WEAK_DEFINES). */
#ifdef FIX_WEAK
#define W __attribute__((weak))
#else
#define W
#endif
W __attribute__((noinline)) int LAPACKE_dgeqrf_work(void) { return 1; }
int LAPACKE_dgeqrf(void) { return LAPACKE_dgeqrf_work(); }
W __attribute__((noinline)) int dlarf1f_(void) { return 1; }
int fix_internal(void) { return dlarf1f_(); }
