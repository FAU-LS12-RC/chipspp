#!/usr/bin/env bash

set -e
set -u

scriptDir="$(dirname "$0")"
projectsDir="$scriptDir"/generated-models/temp
#avoid colons
resultsDir="$scriptDir"/synthesis-results/"$(date +'%Y-%m-%d--%H-%M-%S')"

mkdir "$resultsDir"

for projectFolder in "$projectsDir"/*/; do
  "$scriptDir"/chipsc/bin/chipsc "$projectFolder" --impl "$resultsDir"
  mkdir "$projectFolder"/ip
  "$scriptDir"/chipsc/bin/chipsc "$projectFolder" --ip "$projectFolder"/ip
done

##!/usr/bin/env bash
#
#set -e
#set -u
#
#scriptDir="$(dirname "$0")"
#projectsDir="$scriptDir"/generated-models-tests/temp
##avoid colons
#resultsDir="$scriptDir"/synthesis-results/"$(date +'%Y-%m-%d--%H-%M-%S')"
#
#mkdir "$resultsDir"
#
#for projectFolder in "$projectsDir"/*/; do
#  chipName="$(basename "$projectFolder")"
#  chipName="${chipName##*-}"
#  chipName="${chipName%.c}"
#
#  "$scriptDir"/synthesize-project.sh "$resultsDir" "$projectFolder" "$chipName"
#done
