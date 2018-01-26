
#!/bin/sh -u
# set FCCSWBASEDIR to the directory containing this script
export FCCSWBASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source /cvmfs/fcc.cern.ch/testing/sw/views/stable/x86_64-slc6-gcc62-opt/setup.sh
export CMAKE_PREFIX_PATH=/afs/cern.ch/work/v/vavolkl/public/fcc.cern.ch/sw/0.9.0/tricktrack/install:$CMAKE_PREFIX_PATH

