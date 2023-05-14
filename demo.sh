#!bin/bash

datetime=`date "+%Y%m%d%H%M"`
output_dir="output/${datetime}"
log_file=logs/${datetime}
mkdir ${output_dir}


################################################################ ARGUMENTS ################################################################

camera_id=1
capture_seconds=1
frames_per_second=1                                                                                         # for the UEYE LE model:'min'=3,'max'=58
settings_file='ueye/parameters/green_test.ini'                                                              # File to adjust camera settings
                                                                                                            

################################################### Immediate future work addressed argument ################################################

# # resolution='772,519'                                                                                      # comma separated width and height. Not implemented yet

####################################################### Future work addressed arguments #####################################################
# Note:current implementation supports tuning of parameters via the ini file
# Note:(settings_file) and (autofocus_min,autofocus_max,automatic_gain_max) are mutually exclusive. 
# Note:If settings_file is provided (or even both groups), the settings stored in the file will take place.
# Note:the setting of these parameters will allow the camera to adjust its settings automatically
# Note:implementation using the IDS api is already provided but not supported via arguments yet.

# # autofocus_min=297
# # autofocus_max=297 
# # automatic_gain_max=100

###########################################################################################################################################

args=""
if [ ! -z ${1:-${datetime}} ]; then args="${args} --datetime ${datetime}"; fi
if [ ! -z ${2:-${camera_id}} ]; then args="${args} --camera_id ${camera_id}"; fi
if [ ! -z ${3:-${capture_seconds}} ]; then args="${args} --capture_seconds ${capture_seconds}"; fi
if [ ! -z ${4:-${frames_per_second}} ]; then args="${args} --frames_per_second ${frames_per_second}"; fi
if [ ! -z ${5:-${resolution}} ]; then args="${args} --resolution ${resolution}"; fi
if [ ! -z ${6:-${output_dir}} ]; then args="${args} --output_dir ${output_dir}"; fi
if [ ! -z ${7:-${settings_file}} ]; then args="${args} --settings_file ${settings_file}"; fi
if [ ! -z ${8:-${autofocus_min}} ]; then args="${args} --autofocus_min ${autofocus_min}"; fi
if [ ! -z ${9:-${autofocus_max}} ]; then args="${args} --autofocus_max ${autofocus_max}"; fi
if [ ! -z ${10:-${automatic_gain_max}} ]; then args="${args} --automatic_gain_max ${automatic_gain_max}"; fi

echo "Bash args--> ${args}"

./ueye/bin/record_video ${args} | tee ${log_file}