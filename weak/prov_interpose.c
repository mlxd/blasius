/* __interpose shim (macos-dyld-override §2.2): replaces calls bound to libstub's cblas_ddot. */
#include <mach-o/dyld-interposing.h>
double cblas_ddot(int, const double*, int, const double*, int);
static double my_ddot(int n, const double* x, int ix, const double* y, int iy) { (void)n; (void)x; (void)ix; (void)y; (void)iy; return 7.0; }
DYLD_INTERPOSE(my_ddot, cblas_ddot)
