#!/usr/bin/env bash

dir="$(dirname "$0")"
#llc=$SCR/llvm-6-install/bin/llc
llc=llc

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
