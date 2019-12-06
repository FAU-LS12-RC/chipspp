#!/usr/bin/env bash

#dir="$(dirname "$0")"
dir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

opt -load "$dir"/LLVMChipsOpt.so -chipsopt <"$1" >"$1".opt

#llvm-dis main.bc.opt -o - | less
