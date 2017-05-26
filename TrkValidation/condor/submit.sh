export FILENAME=$1
export FILENAME_DATA=${FILENAME/options/data}
condor_submit TrkValidation/condor/submit.sub -append arguments=$FILENAME -append error=${FILENAME/.py/.err} -append output=${FILENAME/.py/.out} -append log=${FILENAME/.py/.log}

