#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

make ARG=global_calib_AC_lr

OUT_BASE="${1:-./scan_outputs}"
GLOBAL_BASE="$OUT_BASE/global_calib"
mkdir -p "$GLOBAL_BASE"

mapfile -t BETAS < <(python3 - <<'PY'
vals = [i/1000 for i in range(1, 11)] + [i/100 for i in range(2, 11)]
for v in vals:
    print(f"{v:.3f}")
PY
)

RUNS=(11538 11537 11543 11544)
CHS=(4 6 12 14)
MID=9

for beta in "${BETAS[@]}"; do
    beta_tag="beta_${beta}"
    beta_dir="$GLOBAL_BASE/$beta_tag/time_calib"
    mkdir -p "$beta_dir"

    for idx in "${!CHS[@]}"; do
        runnum="${RUNS[$idx]}"
        ch="${CHS[$idx]}"
        out_file="$beta_dir/global_AC_MID_${MID}_ch_${ch}.txt"
        ./global_calib_AC_lr_program "$runnum" "$MID" "$ch" "$beta" "$out_file"
    done

done

echo "Done: global calibration beta scan outputs under $GLOBAL_BASE"
