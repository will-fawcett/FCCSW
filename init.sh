#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source /cvmfs/fcc.cern.ch/sw/0.8.1/init_fcc_stack.sh $1

export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:/build/vavolkl/FCC/a-common-tracking-sw/install/share/cmake/ACTS

