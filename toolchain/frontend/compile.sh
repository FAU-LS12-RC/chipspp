#!/usr/bin/env bash

#dir="$(dirname "$0")"
dir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

declare -a args=(
  -ffreestanding
  -fno-builtin
  -nostdinc
  -target sparc-none-none 
)
includeFile="$dir"/llvm-defs.h

clang=clang
case "$1" in
  *.cpp) clang=clang++;;
esac

output="$(basename "$1")"
output="${output%.*}".bc

#-flto = -emit-llvm
"$clang" -flto -c "${args[@]}" \
  -I "$dir"/../../chips-stable-installation/lib/python2.7/site-packages/chips/compiler/include \
  -include "$includeFile" \
  -O0 -o "$output" "$@"

# -ffreestanding -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -fno-tree-loop-distribute-patterns
# CXXFLAGS = -std=c++11 -m32 -I. $(OPTFLAGS) $(WARNFLAGS) $(STANDALONEFLAGS) $(FLOATINGPOINT) -Wno-non-virtual-dtor -fno-rtti -fno-exceptions -Wno-write-strings -fno-stack-protector -mno-red-zone 
