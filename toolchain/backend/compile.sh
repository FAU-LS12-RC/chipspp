#!/usr/bin/env bash

dir="$(dirname "$0")"
#llc=$SCR/llvm-6-install/bin/llc
#llc=/usr/bin/llc
llc=llc
#llc=$SCR/chips++/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/llc
#llc=$SCR/llvm-install-2/bin/llc

miSched=(-enable-misched -misched-postra)

#"$llc" --load "$dir"/libLLVMChipsCodeGen.so -march sparc "$@" #-print-machineinstrs #-view-dag-combine1-dags \

"$llc" --load "$dir"/libLLVMChipsCodeGen.so -march chips -mcpu chips \
  "$@"
  #-print-machineinstrs #-view-dag-combine1-dags \
#-debug-only=post-RA-sched 
#-debug-only=machine-scheduler
#llc --load "$dir"/libLLVMChipsCodeGen.so -march chips "$@" #-view-dag-combine1-dags \
  #-debug \
  #-view-dag-combine1-dags #-view-legalize-dags
