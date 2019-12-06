#!/usr/bin/env bash

dir="$(dirname "$0")"

opt -load "$dir"/LLVMChipsStackAnalysis.so -chipsstack <"$1" >/dev/null
