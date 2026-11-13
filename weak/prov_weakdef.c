/* The shim from macos-dyld-override §1.2: strong cblas_ddot plus one dummy weak
 * definition, which sets MH_WEAK_DEFINES so dyld's weak coalescing considers it. */
double cblas_ddot(int n, const double* x, int ix, const double* y, int iy) { (void)n; (void)x; (void)ix; (void)y; (void)iy; return 42.0; }
__attribute__((weak)) int blasius_shim_dummy_weak(void) { return 0; }
