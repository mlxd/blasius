/* Extension module that depends on @rpath/libblasius.0.1.dylib and has no LC_RPATH. */
#include <Python.h>
int blasius_probe_version(void);
static struct PyModuleDef def = {PyModuleDef_HEAD_INIT, "probe_ext", 0, -1, 0};
PyMODINIT_FUNC PyInit_probe_ext(void) { if (blasius_probe_version() != 1) return 0; return PyModule_Create(&def); }
