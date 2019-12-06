#!/usr/bin/env bash

dir="$(dirname "$0")"

. "$dir"/chips-stable-installation/bin/activate
"$@"
