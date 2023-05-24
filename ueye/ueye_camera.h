#ifndef CAMERA_INCLUDED
#define CAMERA_INCLUDED
#include "settings.h"
#include "ueye.h"


class Ueye_camera{
  public:

    Ueye_camera();
    Ueye_camera(Settings &s1);
    ~Ueye_camera();
    Ueye_camera(int id,int testing); //erase when done or replace if valid

    void camera_info(); //request camera information 
    void set_parameters(double newfps);
    void save_parameters(std::string strfilename);
    void load_parameters(std::string strfilename);

    //automatic image control
    void apply_automatic_image_control(); //for white balance - wont be used
    void apply_automatic_image_control(double max_gain);
    
    void disable_automatic_image_control();
    void apply_automatic_focus(int afmin,int afmax);

    //sequential capturing of images
    void prepare_video(int capture_seconds);
    void capture_video(std::string savedir, bool save_frames);
    void stop_video();
    

  protected:
    int nRet;
    void method_report(int nret, std::string method);
    
    //set_parameters
    void set_display_size(int maxX,int maxY);

    //apply_automatic_image_control
    void auto_sexposure_gainc();//for white balance - wont be used
    void auto_sexposure_gainc(double max_gain);
    void auto_white_balance();  
    
    void get_cameras_connected();
    UEYE_CAMERA_LIST* pucl;
  private:
    //Constructor
    Settings s;
    int camera_id;
    HIDS hCam; 
    HWND hWndDisplay;

    //camera_info
    SENSORINFO sInfo;
    int MaxImageSizeX,MaxImageSizeY; // get_sensor_info

    //set_parameters
    int DisplayWidth,DisplayHeight; // Set color mode
    int nColorMode,nBitsPerPixel; // Set display mode
    double fps,actualfps; // Set FrameRate

    //prepare video
    int	nNumberOfBuffers;
    int seconds_of_capture;
    std::vector<INT> viSeqMemId;		// camera memory - buffer IDs
	  std::vector<char*> vpcSeqImgMem;	// camera memory - pointers to buffer

    char* pcImageMemory;
    
};

#endif
