#!/usr/bin/env bash

dir="$(dirname "$0")"

newest() {
  ls -dt "$@" | head -1
}

declare -a folders=(
  "$(newest /tmp/llvm*O3*int_adder*/)"
  "$(newest /tmp/python*int_adder*/)"

  "$(newest /tmp/llvm*Os*quicksort.c*/)"
  "$(newest /tmp/python*quicksort*/)"
  "$(newest /tmp/llvm*Os*quicksort8*/)"
  "$(newest /tmp/llvm*Os*quicksort16*/)"

  "$(newest /tmp/llvm*O3*quicksort.c*/)"
  "$(newest /tmp/llvm*O3*quicksort8*/)"
  "$(newest /tmp/llvm*O3*quicksort16*/)"

  "$(newest /tmp/llvm*O3*-fft*/)"
  "$(newest /tmp/python*fft*/)"

  "$(newest /tmp/llvm*O3*dijkstra*/)"

  "$(newest /tmp/llvm*O3*idct*/)"
  "$(newest /tmp/python*idct*/)"

  "$(newest /tmp/llvm*O3*pbmsr*/)"
  "$(newest /tmp/llvm*O3*aes*/)"
)

for folder in "${folders[@]}"; do
  echo -n "$folder":'  '
  "$dir"/instr-stats.sh "$folder"
done
#
#declare -a folders=()
#echo "${folders[@]}"
