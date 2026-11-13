"""Item (5) and (7) for one Python. Usage: python probe.py <libblasius path> <ext dir or ''> <step>"""
import ctypes, os, sys

lib, extdir, step = sys.argv[1], sys.argv[2], sys.argv[3]
libc = ctypes.CDLL("/usr/lib/libSystem.B.dylib")
libc.dlsym.restype = ctypes.c_void_p
libc.dlsym.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
RTLD_DEFAULT = ctypes.c_void_p(-2)

if step == "info":
    print(f"  executable   {sys.executable} -> {os.path.realpath(sys.executable)}")
    print(f"  version      {sys.version.split()[0]} ({sys.platform}, framework={bool(__import__('sysconfig').get_config_var('PYTHONFRAMEWORK'))})")
    print(f"  getdlopenflags()={sys.getdlopenflags():#x}  RTLD_LAZY={os.RTLD_LAZY:#x} RTLD_NOW={os.RTLD_NOW:#x} RTLD_GLOBAL={os.RTLD_GLOBAL:#x} RTLD_LOCAL={os.RTLD_LOCAL:#x}")
    print(f"  ctypes.DEFAULT_MODE={ctypes.DEFAULT_MODE:#x}")
    print(f"  DYLD_INSERT_LIBRARIES seen by python: {os.environ.get('DYLD_INSERT_LIBRARIES')!r}")
    sys.exit(0)

sys.path.insert(0, extdir)
if step in ("local", "global"):
    mode = os.RTLD_LOCAL if step == "local" else os.RTLD_GLOBAL
    ctypes.CDLL(lib, mode=mode)
def vis():
    return {n: bool(libc.dlsym(RTLD_DEFAULT, n.encode())) for n in ("blasius_probe_version", "cblas_ddot")}
before = vis()
class DlInfo(ctypes.Structure):
    _fields_ = [("fname", ctypes.c_char_p), ("fbase", ctypes.c_void_p), ("sname", ctypes.c_char_p), ("saddr", ctypes.c_void_p)]
p = libc.dlsym(RTLD_DEFAULT, b"cblas_ddot")
if p and step == "none":
    d = DlInfo(); libc.dladdr.argtypes = [ctypes.c_void_p, ctypes.POINTER(DlInfo)]; libc.dladdr(p, ctypes.byref(d))
    print(f"  cblas_ddot before any preload comes from {d.fname.decode()}")
try:
    import probe_ext  # noqa: F401
    res = "import OK"
except ImportError as e:
    res = "import FAILED: " + str(e).splitlines()[0][:140]
print(f"  preload={step:<6} {res}; RTLD_DEFAULT before import {before}, after {vis()}")
