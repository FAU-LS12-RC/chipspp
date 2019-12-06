#!/bin/bash

#needs pip, virtualenv for python2 each

virtualenv --system-site-packages chips-stable-installation
source ./chips-stable-installation/bin/activate
cd toolchain/Chips-2.0-master/
python setup.py install
