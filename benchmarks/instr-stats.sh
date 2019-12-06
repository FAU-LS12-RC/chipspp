#!/usr/bin/env bash

instrsFile="$1"/instructions
declare -a glob=("$1"/*_main_0.v)
verilogFile="${glob[0]}"

[ -e "$instrsFile" ] || { echo no instructions >&2; exit 1 ; }
[ -e "$verilogFile" ] || { echo no verilog >&2; exit 1 ; }

unique-instrs() {
  cat "$1" | 'sed' -rn '/^[^:]+: ./ { s/^[^:]+: (\S+).*$/\1/; p }' | \
    sort | uniq
}

echo -n 'Unique instructions ('"$(unique-instrs "$instrsFile" | wc -l)"') '
[ "$2" = print ] && { echo; unique-instrs "$instrsFile"; }

#echo Number of instructions: "$(grep '^\[' "$instrsFile" | wc -l)"
instrArrayDecl="$(grep --only-matching -E \
  '\[[0-9]+:[0-9]+\] instructions \[[0-9]+:[0-9]+\]' "$verilogFile")"
echo $(("$(echo "$instrArrayDecl" | sed -r 's/^\[([^:]+).*$/\1/')" + 1)) x \
  $(("$(echo "$instrArrayDecl" | sed -r 's/^.*s \[([^:]+).*$/\1/')" + 1)) bits


#reg [28:0] instructions [16035:0];
