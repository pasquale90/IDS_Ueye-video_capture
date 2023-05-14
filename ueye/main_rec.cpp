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
// #include <filesystem>
#include "ueye.h"
#include "ueye_camera.h"
#include "console.h"
#include "DEFAULTS.h"


inline std::tuple<int,int,int,std::array<int,2>,std::string,std::string> parse_arguments(int argc, char *argv[]){

  InputParser input(argc, argv);

  std::string settings_file;
  int camera_id;
  int capture_seconds;
  int frames_per_second;
  std::array<int,2> resolution;
  std::string output_dir;  
  // const int * autofocus_min;
  // const int * autofocus_max;
  // const int * automatic_gain_max;

  // Initialize video output directory
  if (input.getCmdOption("--output_dir").empty()){                  // if output_dir is not defined
    
    if (!input.getCmdOption("--datetime").empty()){                     // if datetime is defined
      std::string datetime=input.getCmdOption("--datetime");
      output_dir=default_output_dir + "/" + datetime + "/";
    }
    else{                                                               // if datetime is not defined
      output_dir=default_output_dir + "/" + default_datetime + "/";
    }
  }
  else{                                                             // if outputdir is defined
    output_dir=input.getCmdOption("--output_dir");
  }


  // Initialize camera_id
  if (input.getCmdOption("--camera_id").empty()){
    camera_id=default_camera_id;
  }
  else{
    camera_id=std::stoi(input.getCmdOption("--camera_id"));
  }

  // Initialize seconds of capturing
  if (input.getCmdOption("--capture_seconds").empty()){
    capture_seconds=default_capture_seconds;
  }
  else{
    capture_seconds=std::stoi(input.getCmdOption("--capture_seconds"));
  }

  // Initialize fps
  if (input.getCmdOption("--frames_per_second").empty()){
    frames_per_second=default_frames_per_second;
  }
  else{
    frames_per_second=std::stoi(input.getCmdOption("--frames_per_second"));
  }

  // Initialize camera resolution -- default to max
  if (input.getCmdOption("--resolution").empty()){
    resolution[0]=default_max_resolution[0];
    resolution[1]=default_max_resolution[1];
  }
  else{
    throw std::invalid_argument( "--resolution argument is not yet supported..." );
  }

  // Initialize settings of the camera using the parameters file extracted from the IDS GUI application
  if (input.getCmdOption("--settings_file").empty()){
    settings_file=default_settings_file;
  }
  else{
    settings_file=input.getCmdOption("--settings_file");
  }

  // Throw an error if any unsupported arguments are provided..
  if (!input.getCmdOption("--autofocus_min").empty() || !input.getCmdOption("--autofocus_max").empty() || !input.getCmdOption("--automatic_gain_max").empty()){
    throw std::invalid_argument( "--autofocus_min --autofocus_max argument --automatic_gain_max arguments are not yet supported...\nPlease provide a settings file (.ini) to adjust camera settings...." );
  }
  
  return {camera_id,capture_seconds,frames_per_second,resolution,output_dir,settings_file}; // autofocus_min,autofocus_max,automatic_gain_max
}

int main(int argc, char *argv[]){

  std::cout<<"Hello Ueye"<<std::endl;
  // std::string datetime,output_dir,settings_file;
  // int camera_id,capture_seconds,frames_per_second; //frame_resolution,autofocus_min,autofocus_max,automatic_gain_max;
  // tie (datetime,camera_id,capture_seconds,frames_per_second,output_dir,settings_file) = parse_arguments(argc,argv); //autofocus_min,autofocus_max,automatic_gain_max,
  auto [camera_id,capture_seconds,frames_per_second,resolution,output_dir,settings_file] = parse_arguments(argc,argv);
  std::cout<<"camera_id "<<camera_id<<std::endl;
  std::cout<<"capture_seconds "<<capture_seconds<<std::endl;
  std::cout<<"frames_per_second "<<frames_per_second<<std::endl;
  std::cout<<"resolution "<<resolution[0]<<","<<resolution[1]<<std::endl;
  std::cout<<"output_dir "<<output_dir<<std::endl;
  std::cout<<"settings_file "<<settings_file<<std::endl;
  // std::cout<<"autofocus_min "<<autofocus_min<<std::endl;
  // std::cout<<"autofocus_max "<<autofocus_max<<std::endl;
  // std::cout<<"automatic_gain_max "<<automatic_gain_max<<std::endl;

  Ueye_camera ueye=Ueye_camera(camera_id);

  ueye.load_parameters(settings_file);
  ueye.camera_info();
  ueye.set_parameters(frames_per_second); //(float diplay_to_max_ratio, double newfps)


  // aply autofocus
  // std::cout<<"Executing automatic focus"<<std::endl;
  // ueye.apply_automatic_focus(autofocus_min,autofocus_max);
  // ueye.prepare_video(capture_seconds*2); // x2 to give time for the camera to adjust properly
  // ueye.capture_video(tempdir,false); 
  // ueye.stop_video();

  // apply image control
  // std::cout<<"Executing white balance"<<std::endl;
  // ueye.apply_automatic_image_control(automatic_gain_max);
  // ueye.disable_automatic_image_control();
  // //capture to apply image control
  // ueye.prepare_video(capture_seconds*2); // x2 to give time for the camera to adjust properly
  // ueye.capture_video(tempdir,false);
  // ueye.stop_video();
  
  //capture video
  ueye.prepare_video(capture_seconds);
  ueye.capture_video(output_dir,true);
  ueye.stop_video();

  ueye.~Ueye_camera();  
  std::cout<<"Exiting application"<<std::endl;

  return 0;
}
