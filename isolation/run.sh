#!/bin/bash
# Item (8): do two-level namespaces keep a RTLD_LOCAL provider's own calls inside it?
# Env: BREW_OB (Homebrew libopenblas.dylib), SCIPY32 / SCIPY64 (scipy-openblas dylibs).
set -u
cd "$(dirname "$0")"; B=$PWD/build; mkdir -p $B
cc() { clang -O1 -w "$@" || echo "BUILD FAILED: $*"; }

echo "== fixtures (consumer = executable; global = linked into it, or dlopen RTLD_GLOBAL)"
for w in strong weak; do d=; [ $w = weak ] && d=-DFIX_WEAK
  cc -dynamiclib $d -install_name @rpath/libfixintr_$w.dylib -o $B/libfixintr_$w.dylib fixintr.c
  cc -o $B/fixrun_link_$w fixrun.c -L$B -Wl,-needed-lfixintr_$w -Wl,-rpath,$B
done
cc -o $B/fixrun fixrun.c
cc -dynamiclib -O0 -o $B/fix_twolevel.dylib fixprov.c
cc -dynamiclib -O0 -DFIX_WEAK -o $B/fix_weakdef.dylib fixprov.c
cc -dynamiclib -O0 -Wl,-flat_namespace -o $B/fix_flat.dylib fixprov.c
cc -dynamiclib -O0 -Wl,-interposable -o $B/fix_interposable.dylib fixprov.c
for p in twolevel weakdef flat interposable; do
  echo "  fix_$p flags: $(otool -hv $B/fix_$p.dylib | tail -1 | awk '{for(i=8;i<=NF;i++) printf "%s ", $i}')"
  for g in strong weak; do
    echo "    linked $g global:    $($B/fixrun_link_$g $B/fix_$p.dylib 2>&1)"
    echo "    dlopen'd $g global:  $($B/fixrun $B/fix_$p.dylib $B/libfixintr_$g.dylib 2>&1)"
  done
done

echo "== stub stand-ins (every BLASius CBLAS/LAPACKE name, plain and _64)"
for w in strong weak; do f=; [ $w = weak ] && f=--weak
  python3 gen_stubs_mac.py names.txt $B/stubs_$w.c $f
  cc -dynamiclib -install_name @rpath/libstubs_$w.dylib -o $B/libstubs_$w.dylib $B/stubs_$w.c
  cc -O2 -o $B/prov_test_link_$w prov_test_mac.c -L$B -Wl,-needed-lstubs_$w -Wl,-rpath,$B
  echo "  libstubs_$w flags: $(otool -hv $B/libstubs_$w.dylib | tail -1 | awk '{for(i=8;i<=NF;i++) printf "%s ", $i}')"
done
cc -O2 -o $B/prov_test prov_test_mac.c

echo "== provider images"
for p in "$BREW_OB" "$SCIPY32" "$SCIPY64"; do
  echo "  $p"
  echo "    flags: $(otool -hv "$p" | tail -1 | awk '{for(i=8;i<=NF;i++) printf "%s ", $i}')"
  echo "    exports: $(nm -gU "$p" | wc -l | tr -d ' '), weak defs: $(nm -gUm "$p" | grep -c 'weak external')"
  echo "    weak-def names: $(nm -gUm "$p" | grep 'weak external' | awk '{print $NF}' | tr '\n' ' ')"
  echo "    deps: $(otool -L "$p" | tail -n +2 | awk '{print $1}' | tr '\n' ' ')"
done
OB="$BREW_OB|||0"; S32="$SCIPY32|scipy_||0"; S64="$SCIPY64|scipy_|64_|1"
t() { label=$1; shift; out=$(OPENBLAS_NUM_THREADS=2 perl -e 'alarm 300; exec @ARGV' "$@" 2>&1); rc=$?; echo "  [rc=$rc] $label: $(echo "$out" | tail -1)"; }

echo "== 512 KiB default thread stacks (control for the earlier SIGBUS)"
for s in "$S32"; do SMALL_STACK=1 t "$(basename "${s%%|*}") alone, default stack" $B/prov_test --slot "$s"; done
echo "== real providers, RTLD_LOCAL"
for s in "$OB" "$S32" "$S64"; do n=$(basename "${s%%|*}")
  t "$n alone" $B/prov_test --slot "$s"
  t "$n, weak stubs linked" $B/prov_test_link_weak --slot "$s"
  t "$n, strong stubs linked" $B/prov_test_link_strong --slot "$s"
  t "$n, weak stubs dlopen GLOBAL" $B/prov_test --global $B/libstubs_weak.dylib --slot "$s"
done

echo "== intruders (all names exported by a provider, global)"
python3 gen_intruder_mac.py $B/intr_ob.c "$BREW_OB"; cc -dynamiclib -o $B/libintr_ob.dylib $B/intr_ob.c
python3 gen_intruder_mac.py $B/intr_s32.c "$SCIPY32"; cc -dynamiclib -o $B/libintr_s32.dylib $B/intr_s32.c
t "openblas, its own names global (incl. gotoblas_init)" $B/prov_test --global $B/libintr_ob.dylib --slot "$OB"
t "scipy-openblas32, its own names global" $B/prov_test --global $B/libintr_s32.dylib --slot "$S32"
echo "== a second real OpenBLAS in global scope"
t "scipy-openblas32 local, scipy-openblas64 global" $B/prov_test --global "$SCIPY64" --slot "$S32"
t "openblas local, scipy-openblas64 global" $B/prov_test --global "$SCIPY64" --slot "$OB"
t "openblas local, scipy-openblas32 global" $B/prov_test --global "$SCIPY32" --slot "$OB"
echo "== rotating, 8 callers, weak stubs linked"
t "openblas + scipy32 + scipy64 --swap" $B/prov_test_link_weak --slot "$OB" --slot "$S32" --slot "$S64" --callers 8 --iters 4 --n 200 --swap
