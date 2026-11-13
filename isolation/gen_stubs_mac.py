#!/usr/bin/env python3
"""Stand-in for libblasius's stubs: every name in names.txt, plain and _64, as a
function printing "INTRUDED <name>" and exiting 42. --weak makes them weak
definitions, as the real BLASius stubs are (which sets MH_WEAK_DEFINES).

    gen_stubs_mac.py names.txt out.c [--weak]
"""
import sys

names, out = sys.argv[1:3]
attr = "__attribute__((weak)) " if "--weak" in sys.argv else ""
with open(out, "w") as o:
    o.write('#include <unistd.h>\n#include <string.h>\n'
            'static void hit(const char*n){write(1,"INTRUDED ",9);write(1,n,strlen(n));write(1,"\\n",1);_exit(42);}\n')
    for n in sorted(set(open(names).read().split())):
        for s in ("", "_64"):
            o.write(f'{attr}void {n}{s}(void){{hit("{n}{s}");}}\n')
