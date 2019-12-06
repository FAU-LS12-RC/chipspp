#!/usr/bin/env bash

set -e

(
  echo Building Chips
  cd Chips*
  ./install.sh
)

(
  echo Building opt
  cd opt
  cmake .
  make
)

(
  echo Building backend
  cd backend
  cmake .
  make
)
