#!/bin/bash

export jobOption=$1

#path of the software
echo "Export paths"
export SOFTWARE_PATH_AFS=/afs/cern.ch/work/v/vavolkl/FCCSW
export CMTCONFIG=x86_64-slc6-gcc62-opt

#echo "Copy a working directory"
#cp -r $SOFTWARE_PATH_AFS .
cd $SOFTWARE_PATH_AFS

echo "Init"
#source the script
source init.sh x86_64-slc6-gcc62

echo "Cat "${jobOption}
cat ${jobOption}

./run gaudirun.py ${jobOption} 
#make -f TrkValidation/TrkValidationMakefile.mk -j 2

#echo "LS:"
#ls -l
#echo "Copy output file to eos"
#export EOS_MGM_URL="root://eospublic.cern.ch"
#source /afs/cern.ch/project/eos/installation/client/etc/setup.sh
#xrdcp output.root root://eospublic//eos/fcc/users/v/vavolkl/output.root
#
#echo "Cleaning"
#cd ../
#rm -rf FCCSW
