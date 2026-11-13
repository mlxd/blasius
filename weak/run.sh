#!/bin/bash
# Item (4): does DYLD_INSERT_LIBRARIES of a provider override BLASius's weak stub?
set -u
cd "$(dirname "$0")"; B=build; mkdir -p $B
cc() { clang -O1 "$@" || echo "BUILD FAILED: $*"; }
cc -dynamiclib -install_name @rpath/libstub.dylib -o $B/libstub.dylib stub.c
cc -dynamiclib -o $B/prov_strong.dylib prov_strong.c
cc -dynamiclib -o $B/prov_weakdef.dylib prov_weakdef.c
cc -dynamiclib -o $B/prov_interpose.dylib prov_interpose.c -L$B -lstub -Wl,-rpath,$PWD/$B
cc -o $B/main_dyn main.c -L$B -lstub -Wl,-rpath,$PWD/$B           # consumer links libblasius.dylib
cc -o $B/main_static main.c stub.c                                 # BLASius stubs linked into the executable
cc -o $B/main_static_exp main.c stub.c -Wl,-export_dynamic         # same, exported
for f in $B/libstub.dylib $B/prov_strong.dylib $B/prov_weakdef.dylib $B/main_dyn $B/main_static $B/main_static_exp; do
  printf '%-28s flags: %s\n' "$(basename $f)" "$(otool -hv $f | tail -1 | awk '{for(i=8;i<=NF;i++) printf "%s ", $i}')"
done
echo "-- weak-bind entries for cblas_ddot"
for f in $B/main_dyn $B/main_static $B/main_static_exp; do echo "  $(basename $f):"; dyld_info -fixups $f 2>/dev/null | grep -i ddot | sed 's/^/    /'; done
echo "-- results"
for exe in main_dyn main_static main_static_exp; do
  for p in none prov_strong prov_weakdef prov_interpose; do
    if [ $p = none ]; then r=$($B/$exe 2>&1)
    else r=$(DYLD_INSERT_LIBRARIES=$PWD/$B/$p.dylib $B/$exe 2>&1); fi
    printf '  %-16s + %-15s %s\n' "$exe" "$p" "$r"
  done
done
echo "-- DYLD_PRINT_BINDINGS for main_dyn + prov_weakdef (cblas_ddot lines)"
DYLD_PRINT_BINDINGS=1 DYLD_INSERT_LIBRARIES=$PWD/$B/prov_weakdef.dylib $B/main_dyn 2>&1 | grep -i ddot | head -5
echo "-- same via /bin/sh (SIP-protected shell strips DYLD_*)"
DYLD_INSERT_LIBRARIES=$PWD/$B/prov_weakdef.dylib /bin/sh -c "$B/main_dyn"
