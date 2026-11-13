#!/bin/bash
# Items (5) and (7). PYTHONS: newline-separated "label=path" list.
set -u
cd "$(dirname "$0")"; B=$PWD/build; mkdir -p $B
clang -dynamiclib -install_name @rpath/libblasius.0.1.dylib -o $B/libblasius.0.1.dylib libblasius.c
clang -dynamiclib -o $B/marker.dylib marker.c
while IFS='=' read -r label py; do
  [ -z "$label" ] && continue
  echo "=== $label: $py"
  if [ ! -x "$py" ]; then echo "  not installed"; continue; fi
  real=$("$py" -c 'import sys,os;print(os.path.realpath(sys.executable))')
  DYLD_INSERT_LIBRARIES=$B/marker.dylib "$py" probe.py x x info 2>&1 | sed 's/^MARKER-LOADED/  [direct] MARKER-LOADED (DYLD_INSERT_LIBRARIES honoured)/'
  echo "  via /usr/bin/env:  $(DYLD_INSERT_LIBRARIES=$B/marker.dylib /usr/bin/env "$py" -c 'import os;print(os.environ.get("DYLD_INSERT_LIBRARIES"))' 2>&1 | tr '\n' ' ')"
  echo "  via /bin/sh -c:    $(DYLD_INSERT_LIBRARIES=$B/marker.dylib /bin/sh -c "\"$py\" -c 'import os;print(os.environ.get(\"DYLD_INSERT_LIBRARIES\"))'" 2>&1 | tr '\n' ' ')"
  for f in "$py" "$real"; do
    echo "  codesign $(basename "$f") [$f]:"
    codesign -dv "$f" 2>&1 | grep -E 'flags|TeamIdentifier|Signature=' | sed 's/^/    /'
    codesign -d --entitlements - "$f" 2>&1 | grep -oE 'com\.apple\.[a-z.-]+' | sort -u | sed 's/^/    ent: /'
    [ "$f" = "$real" ] && break
  done
  inc=$("$py" -c 'import sysconfig;print(sysconfig.get_paths()["include"])')
  ext=$B/ext-$label; mkdir -p $ext
  if [ -f "$inc/Python.h" ] && clang -bundle -undefined dynamic_lookup -I"$inc" -o $ext/probe_ext.so probe_ext.c -L$B -lblasius.0.1 2>/dev/null; then
    echo "  probe_ext LC_RPATH entries: $(otool -l $ext/probe_ext.so | grep -c LC_RPATH); deps: $(otool -L $ext/probe_ext.so | tail -n +2 | awk '{print $1}' | tr '\n' ' ')"
    for step in none local global; do "$py" probe.py $B/libblasius.0.1.dylib $ext $step 2>&1 | tail -2; done
  else
    echo "  (7) skipped: no Python.h at $inc"
  fi
done <<< "${PYTHONS:-}"
