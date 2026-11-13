#!/bin/bash
# Items (1)-(3): Accelerate symbol families, LAPACKE presence, BLASSetThreading reach.
set -u
cd "$(dirname "$0")"; B=$PWD/build; mkdir -p $B
sw_vers | sed 's/^/  /'; sysctl -n machdep.cpu.brand_string hw.physicalcpu hw.perflevel0.physicalcpu 2>/dev/null | tr '\n' ' '; echo
V=/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A
echo "== export counts from the shared cache (dyld_info -exports)"
for l in libBLAS.dylib libLAPACK.dylib; do
  dyld_info -exports $V/$l > $B/$l.exports 2>&1
  echo "  $l: total $(grep -c '0x' $B/$l.exports), \$NEWLAPACK\$ILP64 $(grep -c 'NEWLAPACK\$ILP64' $B/$l.exports), \$NEWLAPACK only $(grep 'NEWLAPACK' $B/$l.exports | grep -vc 'ILP64'), LAPACKE $(grep -ci lapacke $B/$l.exports), BLAS*Threading $(grep -c 'BLAS[SG]etThreading' $B/$l.exports)"
done
grep -h -i 'lapacke' $B/*.exports | head -5 | sed 's/^/  LAPACKE export: /'
grep -h -E '_lsame' $B/libLAPACK.dylib.exports $B/libBLAS.dylib.exports | sed 's/^/  lsame export: /' | head -6
clang -O2 -o $B/probe_accel probe_accel.c
echo "################ default environment"; $B/probe_accel 2000
echo "################ VECLIB_MAXIMUM_THREADS=1 (control)"; VECLIB_MAXIMUM_THREADS=1 $B/probe_accel 2000 | sed -n '/== (3)/,$p'
