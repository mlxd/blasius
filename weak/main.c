#include <stdio.h>
double cblas_ddot(int, const double*, int, const double*, int);
int main(void) {
    double x = 1, r = cblas_ddot(1, &x, 1, &x, 1);
    printf("cblas_ddot -> %g (%s)\n", r, r == -1 ? "BLASius weak stub" : r == 42 ? "PROVIDER override" : r == 7 ? "INTERPOSED" : "other");
    return 0;
}
