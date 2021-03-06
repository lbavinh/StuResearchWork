#!/bin/bash
export energy=7.7
export model=Reco_UrQMD
# export model=Reco_UrQMD_PWG3-prod9
export working_dir=/weekly/${USER}/lbavinh/PicoDst
# /eos/nica/mpd/users/parfenov/mpd_data/picodst/11.5gev/
# /eos/nica/mpd/users/parfenov/mpd_data/picodst/7.7gev/AuAu/pwg3-prod9/2079123/
# /weekly/parfenov/mpd_winter2019/mpd_prod/7.7gev/picodst/
# /nica/mpd21/parfenov/mpd_winter2019/mpd_prod/picoDst_7.7gev_7M/
echo "Initializing list of list (Inception)"
mkdir -p $working_dir/split/${model}_${energy}

echo "Listing all ROOT tree of $model at $energy GeV (Inception - level 1)"
# find /eos/nica/mpd/users/parfenov/mpd_data/picodst/7.7gev/AuAu/pwg3-prod9/2079123/ -type f -name "*.root" > $working_dir/split/runlist_${model}_${energy}.list
find /weekly/parfenov/mpd_winter2019/mpd_prod/7.7gev/picodst/ -type f -name "*.root" > $working_dir/split/runlist_${model}_${energy}.list
# find /eos/nica/mpd/users/parfenov/mpd_data/picodst/11.5gev/ -type f -name "*.root" > $working_dir/split/runlist_${model}_${energy}.list
# find /nica/mpd21/parfenov/mpd_winter2019/mpd_prod/picoDst_7.7gev_7M/ -type f -name "*.root" > $working_dir/split/runlist_${model}_${energy}.list


echo "Creating list for each job (Inception - level 2)"
split -l100 -d $working_dir/split/runlist_${model}_${energy}.list $working_dir/split/${model}_${energy}/runlist_${model}_${energy}_
for filename in $working_dir/split/${model}_${energy}/runlist_${model}_${energy}_*;do mv "$filename" "$filename.list"; done;

echo "Number of needed job array is:"
ls $working_dir/split/${model}_${energy} | wc -l

echo "Initializing runlist for SGE JOB (Inception - level 3)"
ls $working_dir/split/${model}_${energy}/runlist_${model}_${energy}_* > $working_dir/split/runlistSGE_${model}_${energy}.list

echo "Done. Wake up!"