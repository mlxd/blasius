/* What sits in global scope: BLASius's stub (LAPACKE_dgeqrf_work) and a leaked internal (dlarf1f_). */
#ifdef FIX_WEAK
#define W __attribute__((weak))
#else
#define W
#endif
W int LAPACKE_dgeqrf_work(void) { return 99; }
W int dlarf1f_(void) { return 99; }
