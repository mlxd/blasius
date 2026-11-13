/* Stand-in for libblasius: a weak cblas_ddot, as BLASius's exported stubs are weak. */
__attribute__((weak)) double cblas_ddot(int n, const double* x, int ix, const double* y, int iy) { (void)n; (void)x; (void)ix; (void)y; (void)iy; return -1.0; }
