#!/bin/bash

export energy=UrQMD_7.7_Reco
export working_dir=/mnt/pool/5/$USER/QCumulant/build
echo "Initializing list of list (Inception)"
mkdir -p $working_dir/split/${energy}
echo "Listing all ROOT tree of $energy GeV (Inception - level 1)"
find /mnt/pool/nica/6/parfenovpeter/mpd_data/PicoDst/7.7gev/ncx/ -type f -name "*.root" > $working_dir/split/runlist_${energy}.list
echo "Creating list for each job (Inception - level 2)"
split -l349 -d $working_dir/split/runlist_${energy}.list $working_dir/split/${energy}/runlist_${energy}_
for filename in $working_dir/split/${energy}/runlist_${energy}_*;do mv "$filename" "$filename.list"; done;
echo "Number of needed job array is:"
ls $working_dir/split/${energy} | wc -l
echo "Initializing runlist for SGE JOB (Inception - level 3)"
ls $working_dir/split/${energy}/runlist_${energy}_*.list > $working_dir/split/runlistSGE_${energy}.list
echo "Done. Wake up!"
