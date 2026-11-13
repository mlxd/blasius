/* Inserted-library marker: says so on stderr when dyld loads it. */
#include <unistd.h>
__attribute__((constructor)) static void hello(void) { write(2, "MARKER-LOADED\n", 14); }
