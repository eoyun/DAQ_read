#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

make ARG=pt_test_AC_global
make ARG=time_calib
make ARG=time_calib_drs

OUT_BASE="${1:-./scan_outputs}"
GLOBAL_BASE="$OUT_BASE/global_calib"
VALID_BASE="$OUT_BASE/validation"
PT_BASE="$VALID_BASE/pt"
SP_BASE="$VALID_BASE/sp"
mkdir -p "$PT_BASE" "$SP_BASE"

SUMMARY_CSV="$VALID_BASE/pt_sigma_summary.csv"
echo "beta,mid,ch,runnum,sigma_ns,root_file" > "$SUMMARY_CSV"

MID=9
RUNS=(11538 11537 11544 11543)
CHS=(4 6 12 14)

mapfile -t BETAS < <(python3 - <<'PY'
vals = [i/1000 for i in range(1, 11)] + [i/100 for i in range(2, 11)]
for v in vals:
    print(f"{v:.3f}")
PY
)

for beta in "${BETAS[@]}"; do
    beta_tag="beta_${beta}"
    beta_pt_dir="$PT_BASE/$beta_tag"
    beta_sp_single_dir="$SP_BASE/$beta_tag/single_chip"
    beta_sp_multi_dir="$SP_BASE/$beta_tag/multi_chip"
    mkdir -p "$beta_pt_dir" "$beta_sp_single_dir" "$beta_sp_multi_dir"

    calib_dir="$GLOBAL_BASE/$beta_tag/time_calib"

    for idx in "${!CHS[@]}"; do
        runnum="${RUNS[$idx]}"
        ch="${CHS[$idx]}"
        calib_file="$calib_dir/global_AC_MID_${MID}_ch_${ch}.txt"
        out_root="$beta_pt_dir/pt_test_run_${runnum}_mid_${MID}_ch_${ch}.root"

        sigma_line=$(./pt_test_AC_global_program "$runnum" "$MID" "$ch" "$calib_file" "$out_root" | awk '/PT_PERIOD_SIGMA_NS/ {print $2}' | tail -n 1)
        sigma_val="${sigma_line:-nan}"
        echo "$beta,$MID,$ch,$runnum,$sigma_val,$out_root" >> "$SUMMARY_CSV"
    done

    # SP single-chip: (run, mid, ch1, ch2) = (11546, 9, 4, 6)
    ./time_calib_program \
        11546 9 4 6 \
        "$calib_dir/global_AC_MID_9_ch_4.txt" \
        "$calib_dir/global_AC_MID_9_ch_6.txt" \
        "$beta_sp_single_dir/time_calib_runnum_11546_mid_9.root"

    # SP multi-chip: (run, mid, ch1, ch2, ch1_ref, ch2_ref) = (11545, 9, 4, 12, 6, 14)
    ./time_calib_drs_program \
        11545 9 4 12 6 14 \
        "$calib_dir/global_AC_MID_9_ch_4.txt" \
        "$calib_dir/global_AC_MID_9_ch_12.txt" \
        "$calib_dir/global_AC_MID_9_ch_6.txt" \
        "$calib_dir/global_AC_MID_9_ch_14.txt" \
        "$beta_sp_multi_dir/time_calib_drs_runnum_11545_mid_9.root"

done

echo "Done: validations under $VALID_BASE"
echo "PT sigma summary: $SUMMARY_CSV"
