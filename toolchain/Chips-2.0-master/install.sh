#!/usr/bin/env bash

dir="$(dirname "$0")"

"$dir"/../../with-chips.sh python setup.py install
