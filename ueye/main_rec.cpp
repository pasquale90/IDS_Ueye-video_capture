#include <iostream>
#include <vector>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <tuple>
#include <cstring>
#include <iterator>
#include <string>
#include <sstream>
#include <functional>
#include <algorithm>
// #include <filesystem>
#include "ueye.h"
#include "ueye_camera.h"
#include "console.h"
#include "settings.h"
// #include "DEFAULTS.h"



// Split a string
std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
    // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
  }
  return elems;
}

// inline std::tuple<int,int,int,std::pair<int,int>,std::string,std::string> parse_arguments(int argc, char *argv[]){
void parse_arguments(int argc, char *argv[],Settings &s){

  InputParser input(argc, argv);

  // Initialize video output directory
  if (input.getCmdOption("--output_dir").empty()){                  // if output_dir is not defined
    
    if (!input.getCmdOption("--datetime").empty()){                     // if datetime is defined
      std::string datetime=input.getCmdOption("--datetime");
      s.output_dir=s.output_dir + "/" + s.datetime + "/";
    }
    else{                                                               // if datetime is not defined
      s.output_dir=s.output_dir + "/" + s.datetime + "/";
    }
  }
  else{                                                             // if outputdir is defined
    s.output_dir=input.getCmdOption("--output_dir");
  }


  // Initialize camera_id
  if (!input.getCmdOption("--camera_id").empty()){
    s.camera_id=std::stoi(input.getCmdOption("--camera_id"));
  }

  // Initialize seconds of capturing
  if (!input.getCmdOption("--capture_seconds").empty()){
    s.capture_seconds=std::stoi(input.getCmdOption("--capture_seconds"));
  }

  // Initialize fps
  if (!input.getCmdOption("--frames_per_second").empty()){
    s.frames_per_second=std::stoi(input.getCmdOption("--frames_per_second"));
  }

  // Initialize camera resolution -- default to max
  if (!input.getCmdOption("--resolution").empty()){
    std::string strres=input.getCmdOption("--resolution");
    std::vector<std::string> res{split(strres,'x')};
    for (int i=0; i<res.size(); i++)
    {
      int num = atoi(res.at(i).c_str());
      s.resolution.at(i)=num;
    }
  }

  // Initialize settings of the camera using the parameters file extracted from the IDS GUI application
  if (!input.getCmdOption("--settings_file").empty()){
    s.settings_file=input.getCmdOption("--settings_file");
  }

  // Initialize autofocus min max -- default to max range
  if (!input.getCmdOption("--autofocus_minmax").empty()){
    std::string strres2=input.getCmdOption("--autofocus_minmax");
    std::vector<std::string> res2{split(strres2,',')};
    for (int i=0; i<res2.size(); i++)
    {
      int num = atoi(res2.at(i).c_str());
      s.autofocus_minmax.at(i)=num;
    }
  }

  // Initialize settings of the camera using the parameters file extracted from the IDS GUI application
  if (!input.getCmdOption("--automatic_gain_max").empty()){
    s.automatic_gain_max=std::stoi(input.getCmdOption("--automatic_gain_max"));
  }

  if (!input.getCmdOption("--auto_adjustment").empty()){
    std::string strres3=input.getCmdOption("--auto_adjustment");
    std::vector<std::string> res3{split(strres3,',')};
    for (int i=0; i<res3.size(); i++)
    {
      int num = atoi(res3.at(i).c_str());
      std::cout<<"num["<<i<<"]="<<num<<std::endl;
      s.auto_adjustment.at(i)=num;
    }
  }

  // Throw an error if any unsupported arguments are provided..
  // if (!input.getCmdOption("--autofocus_min").empty() || !input.getCmdOption("--autofocus_max").empty() || !input.getCmdOption("--automatic_gain_max").empty()){
  //   throw std::invalid_argument( "--autofocus_min --autofocus_max argument --automatic_gain_max arguments are not yet supported...\nPlease provide a settings file (.ini) to adjust camera settings...." );
  // }
  
}

int main(int argc, char *argv[]){
  
  std::cout<<"Hello Ueye"<<std::endl;
  // std::string datetime,output_dir,settings_file;
  // int camera_id,capture_seconds,frames_per_second; //frame_resolution,autofocus_min,autofocus_max,automatic_gain_max;
  // tie (datetime,camera_id,capture_seconds,frames_per_second,output_dir,settings_file) = parse_arguments(argc,argv); //autofocus_min,autofocus_max,automatic_gain_max,
  // auto [camera_id,capture_seconds,frames_per_second,resolution,output_dir,settings_file]
  Settings s{};
  parse_arguments(argc,argv,s);

  std::cout<<"camera_id "<<s.camera_id<<std::endl;
  std::cout<<"capture_seconds "<<s.capture_seconds<<std::endl;
  std::cout<<"frames_per_second "<<s.frames_per_second<<std::endl;
  std::cout<<"resolution "<<s.resolution.at(0)<<","<<s.resolution.at(1)<<std::endl;
  std::cout<<"output_dir "<<s.output_dir<<std::endl;
  std::cout<<"settings_file "<<s.settings_file<<std::endl;
  std::cout<<"autofocus_minmax "<<s.autofocus_minmax.at(0)<<", "<<s.autofocus_minmax.at(1)<<std::endl;
  std::cout<<"automatic_gain_max "<<s.automatic_gain_max<<std::endl;
  std::cout<<"auto_adjustment "<<s.auto_adjustment.at(0)<<", "<<s.auto_adjustment.at(1)<<std::endl;

  Ueye_camera ueye=Ueye_camera(s);

  ueye.load_parameters(s.settings_file);
  ueye.camera_info();
  ueye.set_parameters(s.frames_per_second); //(float diplay_to_max_ratio, double newfps)

  // aply autofocus
  if (s.auto_adjustment.at(0)){
    std::cout<<"Executing automatic focus"<<std::endl;
    ueye.apply_automatic_focus(s.autofocus_minmax.at(0),s.autofocus_minmax.at(1));
    ueye.prepare_video(30); // 30 frames to give time for the camera to adjust
    ueye.capture_video("",false); 
    ueye.stop_video();
  }

  // apply image control
  if (s.auto_adjustment.at(1)){
    std::cout<<"Executing white balance"<<std::endl;
    ueye.apply_automatic_image_control(s.automatic_gain_max);
    ueye.disable_automatic_image_control();
    //capture to apply image control
    ueye.prepare_video(30); // 30 frames to give time for the camera to adjust
    ueye.capture_video("",false);
    ueye.stop_video();
  }

  //capture video
  ueye.prepare_video(s.capture_seconds);
  ueye.capture_video(s.output_dir,true);
  ueye.stop_video();

  // ueye.~Ueye_camera();  
  std::cout<<"Exiting application"<<std::endl;

  return 0;
}
