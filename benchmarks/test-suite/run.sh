#!/usr/bin/env bash

dir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

declare -a cmd=(python "$dir"/endtoendtest.py "$@")
#echo "$*"
#if [[ "$*" != *only-vivado* ]]; then
  cmd=("$dir"/../../with-chips.sh "${cmd[@]}")
#fi
PYTHONPATH="$dir"/../chipsc:"$PYTHONPATH" "${cmd[@]}"
