# IDS_Ueye-video_capture
sequential frame capturing using the IDS api for Linux

## Instructions for running

### Install ueye

### open uEye Demo Suite
 - Note down the camera id, and open camera
 - use ueye->settings option from the menu bar and adjust camera settings
 - Use File->save_parameter->as_a_file,  select a location for storing and save it under the ".ini" extension
 - Open a terminal and locate to the root dir of this repo
 - Initialize the arguments accordingly and the run the demo script to capture sequential frames

### Current suppored arguments

 - camera_id
 - capture_seconds
 - frames_per_second
 - settings_file

### Future work
 - resolution
 - support arguments for enabling :
 -- auto white balance (using a timer)
 -- auto focus (autofocus_min,autofocus_max)
 -- automatic adjustement of the brightness intensity (automatic_gain_max)