#include <dlfcn.h>
#include <stdio.h>
int main(int c, char** v) {
    if (c > 2 && !dlopen(v[2], RTLD_NOW | RTLD_GLOBAL)) { printf("GLOBALFAIL %s\n", dlerror()); return 2; }
    void* h = dlopen(v[1], RTLD_NOW | RTLD_LOCAL);
    if (!h) { printf("LOADFAIL %s\n", dlerror()); return 3; }
    int a = ((int (*)(void))dlsym(h, "LAPACKE_dgeqrf"))(), b = ((int (*)(void))dlsym(h, "fix_internal"))();
    printf("LAPACKE_dgeqrf->work: %s   internal->dlarf1f_: %s\n", a == 1 ? "own" : a == 99 ? "CAPTURED" : "?", b == 1 ? "own" : b == 99 ? "CAPTURED" : "?");
    return 0;
}
