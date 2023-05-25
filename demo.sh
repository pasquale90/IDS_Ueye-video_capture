#!bin/bash

datetime=`date "+%Y%m%d%H%M"`
output_dir="output/${datetime}"
log_file=logs/${datetime}
mkdir ${output_dir}

################################################################ ARGUMENTS ################################################################

camera_id=1
capture_seconds=1
frames_per_second=1                                                                                         # for the UEYE LE model:'min'=3,'max'=58
settings_file='ueye/parameters/green_test.ini'                                                              # fps requested. The min is 3.
                                                                                                            # File to adjust camera settings
# Note:
#   current implementation supports tuning of parameters via the ini file.
#   (settings_file) and (autofocus_min,autofocus_max,automatic_gain_max) are mutually exclusive. 
#   If settings_file is provided (or even both groups), the settings stored in the file will take place and finally affect the camera settings.
#   The definition of the following parameters will allow the camera to adjust its settings on-the-fly.
#   Implementation using the IDS api is already provided but not supported via arguments yet.
auto_adjustment='1,0'                                                                                       # 0,0 autofocus,automatic_image_control(gain,whitebalance)
autofocus_minmax='100,112' 
automatic_gain_max=100

####################################################### Future work addressed arguments #####################################################

# resolution='772x519'                                                                                      # 'x' separated width and height.Not supported yet

###########################################################################################################################################

args=""
if [ ! -z ${1:-${datetime}} ]; then args="${args} --datetime ${datetime}"; fi
if [ ! -z ${2:-${output_dir}} ]; then args="${args} --output_dir ${output_dir}"; fi
if [ ! -z ${3:-${camera_id}} ]; then args="${args} --camera_id ${camera_id}"; fi
if [ ! -z ${4:-${capture_seconds}} ]; then args="${args} --capture_seconds ${capture_seconds}"; fi
if [ ! -z ${4:-${frames_per_second}} ]; then args="${args} --frames_per_second ${frames_per_second}"; fi
if [ ! -z ${6:-${resolution}} ]; then args="${args} --resolution ${resolution}"; fi
if [ ! -z ${7:-${settings_file}} ]; then args="${args} --settings_file ${settings_file}"; fi
if [ ! -z ${8:-${autofocus_minmax}} ]; then args="${args} --autofocus_minmax ${autofocus_minmax}"; fi
if [ ! -z ${9:-${automatic_gain_max}} ]; then args="${args} --automatic_gain_max ${automatic_gain_max}"; fi
if [ ! -z ${10:-${auto_adjustment}} ]; then args="${args} --auto_adjustment ${auto_adjustment}"; fi

# cd ueye
# make clean
# make
# cd ..

echo "Bash args--> ${args}"

./ueye/bin/record_video ${args} | tee ${log_file}