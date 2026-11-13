#!/bin/bash
# Shipped-baseline pipeline on Mach-O (blasius_pvt#13 comment on #20): build OpenBLAS as a
# static archive per width, rename the API to bl_<w>_* and localize everything else
# without GNU objcopy, link both widths into one binary, check ddot/dgemm/dgesv.
set -u
cd "$(dirname "$0")"; B=$PWD/build; mkdir -p $B
V=${OPENBLAS_TAG:-v0.3.34}; J=${JOBS:-$(sysctl -n hw.physicalcpu)}
echo "ld: $(ld -v 2>&1 | head -1)"; echo "OpenBLAS TARGET will be auto-detected on: $(sysctl -n machdep.cpu.brand_string)"; echo "clang: $(clang --version | head -1)"
[ -d $B/src ] || git clone -q --depth 1 -b $V https://github.com/OpenMathLib/OpenBLAS $B/src
for W in lp ilp; do
  [ -f $B/ob_$W/libopenblas.a ] && continue
  rm -rf $B/ob_$W; cp -R $B/src $B/ob_$W
  I=; [ $W = ilp ] && I=INTERFACE64=1
  t0=$(date +%s)
  make -s -C $B/ob_$W -j$J NOFORTRAN=1 C_LAPACK=1 NO_SHARED=1 USE_OPENMP=0 NUM_THREADS=64 $I libs netlib > $B/make_$W.log 2>&1 || { echo "OpenBLAS $W build FAILED"; tail -30 $B/make_$W.log; continue; }
  echo "OpenBLAS $V $W built in $(( $(date +%s) - t0 ))s: $(ls -l $B/ob_$W/libopenblas.a | awk '{print $NF}') $(du -h $B/ob_$W/libopenblas_*.a | cut -f1)"
done
# ld-1267 (Xcode 26) requires -platform_version on a raw ld -r; ld-1167 (Xcode 16) did not
PV="-platform_version macos 15.0 $(xcrun --show-sdk-version)"
echo "platform flag: $PV"
names() { nm -gU "$1" 2>/dev/null | awk 'NF==3{print $3}' | sort -u; }
for W in lp ilp; do
  A=$B/ob_$W/libopenblas.a; [ -f $A ] || continue
  names $A > $B/all_$W.txt
  grep -E '^_(cblas_|LAPACKE_)' $B/all_$W.txt > $B/api_$W.txt
  echo "$W: $(wc -l < $B/all_$W.txt) global defs in archive, $(wc -l < $B/api_$W.txt) API (cblas_/LAPACKE_) to rename"
  sed "s/^_\(.*\)/_\1 _bl_${W}_\1/" $B/api_$W.txt > $B/alias_$W.txt
  sed "s/^_/_bl_${W}_/" $B/api_$W.txt > $B/exp_$W.txt
  # Method A: one partial link; -alias_list renames, -exported_symbols_list localizes the rest
  ld -r $PV -arch arm64 -o $B/A_$W.o -all_load $A -alias_list $B/alias_$W.txt -exported_symbols_list $B/exp_$W.txt > $B/A_$W.log 2>&1 \
    && echo "  method A (ld -r -alias_list -exported_symbols_list): OK" || { echo "  method A FAILED:"; head -5 $B/A_$W.log; }
  # Method A-classic: same with the classic linker, if this Xcode still has it
  ld -r -ld_classic $PV -arch arm64 -o $B/Ac_$W.o -all_load $A -alias_list $B/alias_$W.txt -exported_symbols_list $B/exp_$W.txt > $B/Ac_$W.log 2>&1 \
    && echo "  method A with -ld_classic: OK" || { echo "  method A with -ld_classic FAILED: $(head -2 $B/Ac_$W.log | tr '\n' ' ')"; }
  # Method B: partial link first, then llvm-objcopy renames and ld -r localizes
  OC=$(brew --prefix llvm 2>/dev/null)/bin/llvm-objcopy
  sed "s/^_\(.*\)/_\1 _bl_${W}_\1/" $B/api_$W.txt > $B/redef_$W.txt
  if [ -x "$OC" ]; then
    ld -r $PV -arch arm64 -o $B/B0_$W.o -all_load $A > $B/B_$W.log 2>&1 \
    && $OC --redefine-syms=$B/redef_$W.txt $B/B0_$W.o $B/B1_$W.o >> $B/B_$W.log 2>&1 \
    && ld -r $PV -arch arm64 -o $B/B_$W.o $B/B1_$W.o -exported_symbols_list $B/exp_$W.txt >> $B/B_$W.log 2>&1 \
    && echo "  method B (llvm-objcopy --redefine-syms + ld -r -exported_symbols_list): OK" || { echo "  method B FAILED:"; head -5 $B/B_$W.log; }
  else echo "  method B skipped: no llvm-objcopy"; fi
done
for M in A Ac B; do
  [ -f $B/${M}_lp.o ] && [ -f $B/${M}_ilp.o ] || { echo "== method $M: missing objects, no link test"; continue; }
  echo "== method $M: link both widths into one binary"
  if clang -O2 -o $B/bl_test_$M bl_test.c $B/${M}_lp.o $B/${M}_ilp.o > $B/link_$M.log 2>&1; then
    echo "  globals not prefixed bl_ in the binary: $(nm -gU $B/bl_test_$M | awk 'NF==3{print $3}' | grep -vE '^_bl_(lp|ilp)_' | grep -vE '^(__mh_execute_header|_main)$' | wc -l | tr -d ' ')"
    nm -gU $B/bl_test_$M | awk 'NF==3{print $3}' | grep -vE '^_bl_(lp|ilp)_' | grep -vE '^(__mh_execute_header|_main)$' | head -5 | sed 's/^/    leak: /'
    echo "  undefined: $(nm -u $B/bl_test_$M | tr '\n' ' ' | cut -c1-300)"
    OPENBLAS_NUM_THREADS=2 $B/bl_test_$M 2>&1 | sed 's/^/  /'; echo "  rc=${PIPESTATUS[0]}"
  else echo "  LINK FAILED:"; head -8 $B/link_$M.log | sed 's/^/    /'; fi
done
