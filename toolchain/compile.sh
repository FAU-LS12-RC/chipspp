#!/usr/bin/env bash

set -e
set -u

#toolchain="$(dirname "$0")"
toolchain=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

declare -a sources=()
declare -a bcFiles=()
while [ "${1:+"${1:0:1}"}" != - ]; do
  sources+=("$1")
  shift 1
done

allBcBaseName=code

for source_file in "${sources[@]}"; do
  baseName="$(basename "${source_file%.*}")"

  "$toolchain"/frontend/compile.sh "$source_file" "$@"
  llvm-dis "$baseName".bc
  bcFiles+=("$baseName".bc)
done

#llvm-link "${bcFiles[@]}" >"$allBcBaseName".bc
llvm-link -only-needed "${bcFiles[@]}" >"$allBcBaseName".bc
llvm-dis "$allBcBaseName".bc
#TODO extra general optimization pass here?

"$toolchain"/opt/run.sh "$allBcBaseName".bc
llvm-dis "$allBcBaseName".bc.opt

"$toolchain"/backend/compile.sh "$allBcBaseName".bc.opt

"$toolchain"/opt/run-stack-analysis.sh "$allBcBaseName".bc.opt
