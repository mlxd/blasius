/* Stand-in libblasius.0.1.dylib: install name @rpath/libblasius.0.1.dylib, a weak cblas_ddot like the real stubs. */
int blasius_probe_version(void) { return 1; }
__attribute__((weak)) double cblas_ddot(int n, const double* x, int ix, const double* y, int iy) { (void)n; (void)x; (void)ix; (void)y; (void)iy; return -1.0; }
