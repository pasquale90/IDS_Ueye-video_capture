#include <iostream>
#include <string>
#include <tuple>

static std::string default_datetime{"RightNow"};
static int default_camera_id{1};
static int default_capture_seconds{1};
static int default_frames_per_second{58};
static int default_max_resolution[2]={3088,2076};

static std::string default_output_dir{"output"};

static std::string default_settings_file{"ueye/parameters/camera1.ini"};

static int default_autofocus_min{0};
static int default_autofocus_max{1023};
static int default_automatic_gain_max{100};