#!/bin/bash
# Provider discovery checks from "Default search order per platform":
# Homebrew OpenBLAS keg path, and ArmPL _mp by absolute path incl. libomp resolution.
set -u
cd "$(dirname "$0")"; B=$PWD/build; mkdir -p $B
echo "== Homebrew OpenBLAS"
P=$(brew --prefix openblas 2>/dev/null); echo "  brew --prefix openblas: $P   (brew --prefix: $(brew --prefix))"
ls -l $P/lib/libopenblas*.dylib 2>&1 | awk '{print "  " $(NF-2), $(NF-1), $NF}'
echo "  install name: $(otool -D $P/lib/libopenblas.dylib | tail -1)"
for c in /opt/homebrew/lib/libopenblas.dylib /opt/homebrew/opt/openblas/lib/libopenblas.dylib /usr/local/opt/openblas/lib/libopenblas.dylib; do
  printf '  %-52s %s\n' $c "$([ -e $c ] && echo exists || echo absent)"; done
echo "  deps: $(otool -L $P/lib/libopenblas.dylib | tail -n +2 | awk '{print $1}' | tr '\n' ' ')"
echo "== ArmPL"
if [ -z "${ARMPL_TGZ_URL:-}" ]; then echo "  skipped (ARMPL_TGZ_URL unset)"; exit 0; fi
if [ ! -d $B/armpl ]; then
  curl -sL -o $B/armpl.tgz "$ARMPL_TGZ_URL" && tar xzf $B/armpl.tgz -C $B || { echo "  download FAILED"; exit 0; }
  DMG=$(find $B -name '*.dmg' | head -1); echo "  dmg: $(basename "$DMG")"
  hdiutil attach "$DMG" -mountpoint $B/mnt -nobrowse -quiet || { echo "  attach FAILED"; exit 0; }
  INS=$(find $B/mnt -name '*install*.sh' | head -1); echo "  installer: ${INS#$B/mnt/}"
  "$INS" -y --install_dir=$B/armpl > $B/armpl_install.log 2>&1 || { echo "  install FAILED"; tail -5 $B/armpl_install.log; }
  hdiutil detach $B/mnt -quiet
fi
L=$(dirname "$(find $B/armpl -name 'libarmpl_lp64_mp.dylib' | head -1)")
echo "  lib dir: ${L#$B/}"; ls $L | grep -E 'dylib$' | tr '\n' ' '; echo
for f in libarmpl_lp64.dylib libarmpl_lp64_mp.dylib libomp.dylib; do
  echo "  $f: id=$(otool -D $L/$f | tail -1); deps=$(otool -L $L/$f | tail -n +2 | awk '{print $1}' | tr '\n' ' ')"
  echo "    rpaths: $(otool -l $L/$f | awk '/LC_RPATH/{getline;getline;print $2}' | tr '\n' ' ')"
done
clang -O2 -o $B/armpl_probe armpl_probe.c
echo "-- serial libarmpl_lp64 by absolute path"; $B/armpl_probe $L/libarmpl_lp64.dylib
echo "-- libarmpl_lp64_mp by absolute path, nothing preloaded"; $B/armpl_probe $L/libarmpl_lp64_mp.dylib
echo "-- libarmpl_lp64_mp after preloading the bundled libomp by absolute path"; $B/armpl_probe $L/libarmpl_lp64_mp.dylib $L/libomp.dylib
echo "-- libarmpl_lp64_mp after preloading Homebrew libomp"; $B/armpl_probe $L/libarmpl_lp64_mp.dylib "$(brew --prefix libomp 2>/dev/null)/lib/libomp.dylib"
echo "-- libarmpl_lp64_mp with DYLD_LIBRARY_PATH (Arm's documented route)"; DYLD_LIBRARY_PATH=$L $B/armpl_probe $L/libarmpl_lp64_mp.dylib
