/* __interpose shim (macos-dyld-override §2.2): replaces calls bound to libstub's cblas_ddot. */
/* <mach-o/dyld-interposing.h> is in the dyld sources, not the SDK */
#define DYLD_INTERPOSE(_replacement, _replacee) \
    __attribute__((used)) static struct { const void* replacement; const void* replacee; } _interpose_##_replacee \
    __attribute__((section("__DATA,__interpose"))) = {(const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee};
double cblas_ddot(int, const double*, int, const double*, int);
static double my_ddot(int n, const double* x, int ix, const double* y, int iy) { (void)n; (void)x; (void)ix; (void)y; (void)iy; return 7.0; }
DYLD_INTERPOSE(my_ddot, cblas_ddot)
