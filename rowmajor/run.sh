#!/bin/bash
# Item (6): rmtest on Accelerate's $NEWLAPACK routines. The reference is netlib
# LAPACKE 3.12.1, compiled here with its Fortran calls mangled onto the same
# $NEWLAPACK family, so both sides of each comparison run Accelerate's LAPACK.
# Usage: run.sh [lapack-source-dir]   (BUILD_ONLY_MANGLING=plain for a Linux check)
set -u
cd "$(dirname "$0")"; B=$PWD/build; mkdir -p $B
SRC=${1:-$B/lapack-3.12.1}
if [ ! -d "$SRC" ]; then curl -sL https://github.com/Reference-LAPACK/lapack/archive/refs/tags/v3.12.1.tar.gz | tar xz -C $B; fi
L=$SRC/LAPACKE
# LAPACKE entry points rmtest calls, plus their _work and every util
R=$(grep -oE 'LAPACKE_[dz][a-z0-9]+\(' rmtest.c | sed 's/LAPACKE_//; s/(//' | sort -u)
CC=${CC:-clang}
for W in lp64 ilp64; do
  O=$B/$W; mkdir -p $O/inc $O/obj; cp $L/include/*.h $O/inc/
  case "${BUILD_ONLY_MANGLING:-acc}/$W" in
    plain/*)    SFX=; echo '#define LAPACK_GLOBAL(lcname,UCNAME) lcname##_' > $O/inc/lapacke_mangling.h ;;
    acc/lp64)   SFX='$NEWLAPACK'; echo '#define LAPACK_GLOBAL(lcname,UCNAME) lcname##$NEWLAPACK' > $O/inc/lapacke_mangling.h ;;
    acc/ilp64)  SFX='$NEWLAPACK$ILP64'; echo '#define LAPACK_GLOBAL(lcname,UCNAME) lcname##$NEWLAPACK$ILP64' > $O/inc/lapacke_mangling.h ;;
  esac
  D=; [ $W = ilp64 ] && D=-DLAPACK_ILP64
  files=$(for r in $R; do echo $L/src/lapacke_$r.c $L/src/lapacke_${r}_work.c; done; ls $L/utils/*.c | grep -v lapacke_lsame.c; echo lapacke_lsame.c)
  for f in $files; do $CC -O2 -w $D -I$O/inc -I$L/include -c $f -o $O/obj/$(basename $f .c).o & done; wait
  ar rcs $O/liblapacke_ref.a $O/obj/*.o
  if [ -n "$SFX" ]; then
    $CC -O2 -w $D "-DACC_SFX=\"$SFX\"" -I. -I$O/inc -o $O/rmtest rmtest.c $O/liblapacke_ref.a -framework Accelerate || { echo "BUILD FAILED ($W)"; continue; }
    echo "== $W: undefined Accelerate symbols the reference + rmtest bind to"
    nm -u $O/rmtest | grep -c 'NEWLAPACK' | sed 's/^/  count: /'
    nm -u $O/rmtest | grep -v 'NEWLAPACK' | grep -vE '^_(_|abort|calloc|free|malloc|memcpy|memcmp|memset|printf|puts|putchar|fprintf|snprintf|atoi|cabs|fmax|fabs|sqrt|exit|stderr|fflush|fputs|fwrite|realloc|bzero|strlen|getenv|hypot|copysign|isnan)' | sed 's/^/  non-NEWLAPACK: /'
  else
    $CC -O2 -w $D -I$O/inc -o $O/rmtest rmtest.c $O/liblapacke_ref.a ${LINKLIBS:--llapack -lcblas -lopenblas} -lm || { echo "BUILD FAILED ($W)"; continue; }
  fi
done
[ "${BUILD_ONLY_MANGLING:-acc}" = plain ] && exit 0
for W in lp64 ilp64; do
  [ -x $B/$W/rmtest ] || continue
  for T in 1 default; do
    echo "################ $W, VECLIB_MAXIMUM_THREADS=$T, n=${N:-200}"
    if [ $T = default ]; then $B/$W/rmtest ${N:-200}; else VECLIB_MAXIMUM_THREADS=$T $B/$W/rmtest ${N:-200}; fi
  done
done
