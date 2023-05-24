#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <algorithm>

struct Settings
{
    std::string datetime{"RightNow"};
    int camera_id{1};
    int capture_seconds{1};
    int frames_per_second{58};
    std::vector<int> resolution={3088,2076};

    std::string output_dir{"output"};

    std::string settings_file{"ueye/parameters/camera1.ini"};

    int autofocus_min{0};
    int autofocus_max{1023};
    int automatic_gain_max{100};
};


#endif