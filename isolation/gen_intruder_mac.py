#!/usr/bin/env python3
"""All-names intruder from Mach-O exports (nm -gU): functions exit 42 with
"INTRUDED <name>", data names become 4 KiB arrays.

    gen_intruder_mac.py out.c lib.dylib [lib.dylib ...]
"""
import re
import subprocess
import sys

out, *libs = sys.argv[1:]
kind = {}
for p in libs:
    for line in subprocess.run(["nm", "-gU", p], capture_output=True, text=True).stdout.splitlines():
        f = line.split()
        if len(f) < 3 or not f[2].startswith("_"):
            continue
        k, t = f[2][1:], f[1]
        if not re.fullmatch(r"[A-Za-z][A-Za-z0-9_]*", k) or k.startswith(("omp_", "kmp", "GOMP_", "KMP_")):
            continue
        if t in "Tt":
            kind[k] = "f"
        elif t in "DdBbSsCc":
            kind.setdefault(k, "d")
with open(out, "w") as f:
    f.write('#include <unistd.h>\n#include <string.h>\n'
            'static void hit(const char*n){write(1,"INTRUDED ",9);write(1,n,strlen(n));write(1,"\\n",1);_exit(42);}\n')
    for k, t in sorted(kind.items()):
        f.write(f'void {k}(void){{hit("{k}");}}\n' if t == "f" else f"char {k}[4096]={{1}};\n")
print(len(kind))
