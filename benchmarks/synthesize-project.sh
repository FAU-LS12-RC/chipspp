#!/usr/bin/env bash

set -e
set -u

if [ $# -lt 3 ]; then
  echo "Usage: $0 <results-dir> <project-path> <chip-name>" >&2
  exit 2
fi

resultsDir="$1"
path="$2"
chipName="$3"
folderName="$(basename "$path")"
xprPath="$path"/"$folderName".xpr
ipRepoPath="$path"/ip-repo

resourceReportSuffix=_utilization_placed.rpt
timingReportSuffix=_timing_summary_routed.rpt

vivado -mode batch -source synthesize.tcl -tclargs "$xprPath"

implDir="$path"/"$folderName".runs/impl_1

timingReportPath="$implDir"/"$chipName""$timingReportSuffix"
for i in `seq 1 10`; do
  if [ -f "$timingReportPath" ]; then
    break
  fi
  sleep 1
done
cp "$timingReportPath" "$resultsDir"/"${folderName}${timingReportSuffix}"
cp "$implDir"/"${chipName}${resourceReportSuffix}" \
  "$resultsDir"/"${folderName}${resourceReportSuffix}"

vivado -mode batch -source "$path"/package-ip.tcl \
  -tclargs "$xprPath" "$ipRepoPath"

#llvm-O3-quicksort.c.runs/impl_1
#int_adder_timing_summary_routed.rpt
#int_adder_utilization_placed.rpt

