#!/usr/bin/env bash

set -e
set -u

if ! type -P vivado >/dev/null; then
  echo vivado not in PATH
  exit 1
fi

scriptDir="$(dirname "$0")"
chipsDir="$scriptDir"/generated-models/temp

for chipsFolder in "$chipsDir"/*; do
  rm -rf "$chipsFolder"/vivado
  "$scriptDir"/chipsc/bin/chipsc "$chipsFolder" --make-project &
done

wait

#
##!/usr/bin/env bash
#
#set -e
#set -u
#
#if ! type -P vivado >/dev/null; then
#  echo vivado not in PATH
#  exit 1
#fi
#
#scriptDir="$(dirname "$0")"
#chipsDir="$scriptDir"/generated-models/temp
#projectsDir="$scriptDir"/generated-models-tests/temp
#
#rm -r "$projectsDir"
#mkdir "$projectsDir"
#for chipsFolder in "$chipsDir"/*; do
#  "$scriptDir"/make-test-project.sh "$chipsFolder" &
#done
#
#wait
