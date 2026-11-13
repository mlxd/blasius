/* A typical provider: strong exports only, so no MH_WEAK_DEFINES. */
double cblas_ddot(int n, const double* x, int ix, const double* y, int iy) { (void)n; (void)x; (void)ix; (void)y; (void)iy; return 42.0; }
