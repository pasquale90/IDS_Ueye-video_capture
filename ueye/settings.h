#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <algorithm>

struct Settings
{
    std::string datetime{"RightNow"};
    int camera_id{1};
    int capture_seconds{3};
    int frames_per_second{58};
    std::vector<int> resolution={3088,2076};
    std::string output_dir{"output"};
    std::string settings_file{"ueye/parameters/camera1.ini"};
    std::vector<int>  autofocus_minmax{0,1023};
    int automatic_gain_max{100};
    std::vector<int> auto_adjustment{0,0};
};


#endif