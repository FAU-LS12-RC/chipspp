#!/usr/bin/env bash

set -e
set -u

scriptDir="$(dirname "$0")"
programsDir="$scriptDir"/../chips-programs
chipsDir="$scriptDir"/generated-models/temp

declare -a pythonPrograms=(
  int_adder.c:10,
  quicksort.c:300,
  hls_fft.c:7400,
  hls_sqrt.c:,
  idct.orig/idct_py.cpp:900,
)
declare -a llvmPrograms=(
#  int_adder.c:,
#  quicksort.c:200,
#  quicksort8.c:140,
#  quicksort16.c:158,
#  fft.original/fft.c:,

 echo.c:

  #jpegDecoder_chip/mdl_inv_quant_idct1_126_hw/mdl_inv_quant_idct1_126_hw.cpp:,
#  jpegDecoder_chip/mdl_colorspaceconv_4_hw/mdl_colorspaceconv_4_hw.cpp:,
#
#  jpegDecoder_chip/mdl_img_splitter_66_hw/mdl_img_splitter_66_hw.cpp:,
#  jpegDecoder_chip/mdl_inv_zigzag1_200_hw/mdl_inv_zigzag1_200_hw.cpp:,
#  jpegDecoder_chip/mdl_rlc_decoder_inv_zrl1_233_hw/mdl_rlc_decoder_inv_zrl1_233_hw.cpp:,

#  dijkstra/dijkstra.c:1000,7000
#  stringsearch/pbmsrch_small.c:,
  #hls_sqrt.c
)

# all except fifo: 9.5
#fifo: 9.7

#echo: 15-28-51: 6.2
#echo with loop: 15-41-00: 6.2
#addition with i: 15-47-44: 6.2
#addition with r (read from input): 15-51-35: 6.2
#addition and shift by i: 16-30-07: 6.8
#addition and shift by r: 16-38-56: 6.8
#addition and shift and mul by i: 16-: <--- very bad
#addition and shift and mul by i: 08-02 13-: <--- very bad
#idct: 13-22-44: 8.0
#13-36: 6.8
#6.9 with mul worked (08-02-16-59)
#8.0: 08-02-17-12
declare -a baseChipscCall=("$scriptDir"/chipsc/bin/chipsc --clk-period 8.5)

generate() {
  local programPathAndStack="$1"
  local toolchain="$2"
  local optimization="${3:+${3}}"
  [ -z "$optimization" ] && optimization=3
  local programPath="${programPathAndStack%:*}"
  local memorySizes="${programPathAndStack##*:}"
  local heapSize="${memorySizes##*,}"
  local stackSize="${memorySizes%,*}"

  case "$(basename "$programPathAndStack")" in
    quicksort*)
      optimization=s;;
  esac

  echo compiling "$programPath"

  declare -a cmd=("${baseChipscCall[@]}" "$programPath" -o "$chipsDir" --copts=-DASYNCIO)
  cmd+=(--toolchain "$toolchain" --make-subdir -O"$optimization")
  [ -n "$stackSize" ] && cmd+=(--stack-size "$stackSize")
  [ -n "$heapSize" ] && cmd+=(--heap-size "$heapSize")

  "${cmd[@]}"
}

rm -r "$chipsDir"
mkdir "$chipsDir"
#for program in "${pythonPrograms[@]}"; do
#  generate "$programsDir"/python-toolchain/"$program" python
#done
for program in "${llvmPrograms[@]}"; do
  generate "$programsDir"/llvm-toolchain/"$program" llvm
done
#generate "$programsDir"/llvm-toolchain/jpegDecoder_chip/mdl_img_splitter_66_hw/mdl_img_splitter_66_hw.cpp: llvm 0
#generate "$programsDir"/llvm-toolchain/jpegDecoder_chip/mdl_img_splitter_66_hw/mdl_img_splitter_66_hw.cpp: llvm 3
#echo compiling jpeg-decoder-net-O3
"${baseChipscCall[@]}" --make-net \
  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
  --make-subdir --copts=O3,dist -o "$chipsDir"
#echo compiling jpeg-decoder-net-Os
#"${baseChipscCall[@]}" --make-net \
#  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
#  --make-subdir --copts=Os,dist -o "$chipsDir"
#echo compiling jpeg-decoder-net-fifo
#"${baseChipscCall[@]}" --make-net \
#  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
#  --make-subdir --copts=O3,dist,fifo -o "$chipsDir"
#
#echo compiling jpeg-decoder-net-all
"${baseChipscCall[@]}" --make-net \
  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
  --make-subdir --copts=O3,all -o "$chipsDir"
#echo compiling jpeg-decoder-net-external-idct
#"${baseChipscCall[@]}" --make-net \
#  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
#  --make-subdir --copts=O3,external_idct -o "$chipsDir"
#echo compiling jpeg-decoder-net-double-idct
#"${baseChipscCall[@]}" --make-net \
#  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
#  --make-subdir --copts=O3,double_idct -o "$chipsDir"
#echo compiling jpeg-decoder-net-double-idct-doublebuf
#"${baseChipscCall[@]}" --make-net \
#  "$programsDir"/llvm-toolchain/jpeg_decoder.py \
#  --make-subdir --copts=O3,double_idct,doublebuf -o "$chipsDir"
#
"${baseChipscCall[@]}" --toolchain llvm --make-subdir \
  -o "$chipsDir" --copts=-w -O3 \
  "$programsDir"/llvm-toolchain/aes/aesxam.c \
  "$programsDir"/llvm-toolchain/aes/aes.c


#generate "$programsDir"/llvm-toolchain/"fft.original/fft.c:" llvm
#generate "$programsDir"/llvm-toolchain/"jpegDecoder_chip/mdl_inv_quant_idct1_126_hw/mdl_inv_quant_idct1_126_hw.cpp:" llvm

##!/usr/bin/env bash
#
#set -e
#set -u
#
#scriptDir="$(dirname "$0")"
#programsDir="$scriptDir"/../chips-programs
#chipsDir="$scriptDir"/generated-models/temp
#
#declare -a pythonPrograms=(
#  int_adder.c
#  quicksort.c
#  hls_fft.c
#  hls_sqrt.c
#)
#declare -a llvmPrograms=(
#  int_adder.c
#  quicksort.c
#  hls_fft.c
#  #hls_sqrt.c
#)
#
#generate() {
#  local programPath="$1"
#  local toolchain="$2"
#  echo compiling "$programPath"
#  "$scriptDir"/../with-chips.sh "$scriptDir"/generate-chip.py \
#    "$chipsDir" "$programPath" "$toolchain"
#}
#
#rm -r "$chipsDir"
#mkdir "$chipsDir"
#for program in "${pythonPrograms[@]}"; do
#  generate "$programsDir"/python-toolchain/"$program" python
#done
#for program in "${llvmPrograms[@]}"; do
#  generate "$programsDir"/llvm-toolchain/"$program" llvm
#done

