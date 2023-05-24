#include <iostream>
#include <cstring>
#include <vector>
#include "settings.h"
#include "ueye_camera.h"


void Ueye_camera::method_report(int nret, std::string method){
  if (nret !=IS_SUCCESS){
    std::cout<<method<<"::error"<<std::endl;
  }
  else{
   std::cout<<method<<"::success"<<std::endl;
  }
}

// callback function
void print_focus_sharpness(UINT focus, INT sharpness, void*){
  std::cout<<"focus="<<focus<<"\tsharpness="<<sharpness<<std::endl;
}


void Ueye_camera::stop_video(){

  //9. After the end of the image acquisition loop stop the video capture (is_StopLiveVideo).
  
  //stop video capturing
  nRet=is_StopLiveVideo (hCam,IS_FORCE_VIDEO_STOP);
  method_report(nRet,"stop_video::is_StopLiveVideo(hCam,IS_FORCE_VIDEO_STOP)");

  //10.Exit the image queue (is_ExitImageQueue).
  
  //De-inititialize image queue
  nRet=is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_EXIT, NULL, 0);
  method_report(nRet,"stop_video::is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_EXIT, NULL, 0)");

  //11. Clear the buffer sequence (is_ClearSequence).
  //clear sequence
  nRet=is_ClearSequence(hCam);
  method_report(nRet,"stop_video::is_ClearSequence(hCam)");
  
  //12. Deallocate the buffers in a loop (is_FreeImageMem).
  printf("Deallocating %i number of images\n",nNumberOfBuffers);
	for(int k=(nNumberOfBuffers-1); k>=0; k-- )
  {   
    //deallocate image memory(k);
    nRet=is_FreeImageMem(hCam, vpcSeqImgMem.at(k), viSeqMemId.at(k) ) ;
    
    //std::cout<<"Successfully de-allocating image with id {"<<viSeqMemId.at(k)<<"}"<<std::endl;
  }
  method_report(nRet,"stop_video::is_FreeImageMem(hCam, vpcSeqImgMem.at(k), viSeqMemId.at(k) )");
  
  //clean vectors 
  viSeqMemId.clear();
  vpcSeqImgMem.clear();
  nNumberOfBuffers = 0;

}

void Ueye_camera::capture_video(std::string savedir, bool save_frames){

  //6. Start video capture (is_CaptureVideo).

  //activate video recording
  //is_SetExternalTrigger(hCam, IS_SET_TRIGGER_SOFTWARE);
  nRet=is_CaptureVideo(hCam,IS_DONT_WAIT);
  method_report(nRet,"capture_video::is_CaptureVideo");
  
  //7. Acquire the images from camera in a loop:
  
    INT nMemID = 0;
	  char *pBuffer = NULL;

    for (int j=0;j<(int) nNumberOfBuffers;++j){
      //substitute is_ImageQueue(cmd_wait)
      nRet = is_WaitForNextImage(hCam, 1000, &pBuffer, &nMemID);

      if (save_frames){
        
		  if(nRet == IS_SUCCESS)
		  {
			  // do some processing, e.g. image display
        //int nRenderMode=IS_RENDER_FIT_TO_WINDOW;
        int nRenderMode=IS_RENDER_DOWNSCALE_1_2 | IS_RENDER_ROTATE_90;
        (hCam,nMemID,hWndDisplay,nRenderMode);


        //Save the image
        IMAGE_FILE_PARAMS ImageFileParams;
        //wchar_t filename
        //std::string strfilename = "./images/frame_"+std::to_string(nMemID)+".png";
        std::string strfilename;
        if (nMemID<10){
          strfilename = savedir+"/frame_0"+std::to_string(nMemID)+".png";
        }else {
          strfilename = savedir+"/frame_"+std::to_string(nMemID)+".png";
        }
        
        const size_t len = strfilename.length() + 1;
        wchar_t wfname[len];
        swprintf(wfname, len, L"%s", strfilename.c_str());
        ImageFileParams.pwchFileName= wfname;
        UINT a=static_cast<uint>(nMemID);
        ImageFileParams.pnImageID=&a; //NULL; //&nMemoryId
        ImageFileParams.ppcImageMem=&pBuffer;//pcImageMemory;
        ImageFileParams.nQuality=100;
        ImageFileParams.nFileType=IS_IMG_PNG;
        is_ImageFile(hCam,IS_IMAGE_FILE_CMD_SAVE,(void*)&ImageFileParams,sizeof(ImageFileParams));
			
			// do not forget to unlock the buffer, when all buffers are locked we cannot receive images any more
			  is_UnlockSeqBuf (hCam, nMemID, pBuffer);
		  }
    }
  }
}

void Ueye_camera::prepare_video(int capture_seconds){

   //3. Allocate buffers in a loop
    //a. initialize buffers
      //set_number_of_buffers();
  double FrameTimeMin, FrameTimeMax, FrameTimeIntervall;
  
  nRet = is_GetFrameTimeRange (hCam, &FrameTimeMin, &FrameTimeMax, &FrameTimeIntervall);
  if (nRet == IS_SUCCESS)
  {
    double maxBuffers;
    maxBuffers= (int)fps;//(1.0f/FrameTimeMin) +0.5f;
    nNumberOfBuffers = (int) (maxBuffers)*capture_seconds;

    std::cout<<"Total number of buffers"<<nNumberOfBuffers<<std::endl;
    std::cout<<"FrameTimeMin="<<nNumberOfBuffers<<",FrameTimeMax="<<FrameTimeMax<<",FrameTimeIntervall="<<FrameTimeIntervall<<std::endl;
    std::cout<<"Total number of buffers"<<nNumberOfBuffers<<std::endl;
    if( nNumberOfBuffers < 3 ){
      nNumberOfBuffers = 3;
    }
  }
  else std::cout<<"is_GetFrameTimeRange ERROR"<<std::endl;

      //set_buffer_size();//is_AOI// calculate the image buffer width and height , watch if an (absolute) AOI is used
  IS_SIZE_2D imageSize;
  is_AOI(hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));
  INT nAllocSizeX = 0;
  INT nAllocSizeY = 0;
  INT nSizeX = nAllocSizeX = imageSize.s32Width;
  INT nSizeY = nAllocSizeY = imageSize.s32Height;
  UINT nAbsPosX = 0;
  UINT nAbsPosY = 0;
  is_AOI(hCam, IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&nAbsPosX , sizeof(nAbsPosX));
  is_AOI(hCam, IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&nAbsPosY , sizeof(nAbsPosY));
  if (nAbsPosX)
  {
    nAllocSizeX = sInfo.nMaxWidth;
  }
  if (nAbsPosY)
  {
    nAllocSizeY = sInfo.nMaxHeight;
  }


    //b. allocate buffers (memory) in a loop
  int i;
  //char* pcImageMemory;
  char* cpImageMemory;
  int nMemoryId;
  
  printf("Allocating %i number of images\n",nNumberOfBuffers);
  for(i=0;i<nNumberOfBuffers;i++)
  {
    //4.1 Allocate the image memory (is_AllocImageMem).
      //allocate_image_memory();
    nRet=is_AllocImageMem(hCam,MaxImageSizeX,MaxImageSizeY,nBitsPerPixel,&pcImageMemory,&nMemoryId);
    method_report(nRet,"prepare_video::is_AllocImageMem");
    //std::cout<<"Image with id {"<<nMemoryId<<"} has been successfully allocated"<<std::endl;
    
    //4.2 Add the image memory to the sequence (is_AddToSequence).
      //add_to_sequence();
    nRet = is_AddToSequence(hCam, pcImageMemory, nMemoryId);
    if( nRet != IS_SUCCESS ){
      // free latest buffer - deallocate_image_memory();
      nRet=is_FreeImageMem(hCam, vpcSeqImgMem.at(i), viSeqMemId.at(i) ) ;
      if (nRet!=IS_SUCCESS){
        std::cout<<"Cannot immediately deallocate image memory - error within error"<<nRet<<std::endl;
      }
      //else std::cout<<"Immediately de-allocating image with id {"<<nMemoryId<<"}"<<std::endl;
    }
      //fill_vectors(); // store current number buffers in case we did not match to get the desired number
    viSeqMemId.push_back(nMemoryId);
    vpcSeqImgMem.push_back(pcImageMemory);
    
  }
    //store_current_buffer(i);
  nNumberOfBuffers = i;
  

  //5. Initialize image queue (is_InitImageQueue). // enable the image queue init_image_queue(); // call init queue (... before addToSequence ????) 
  //nRet = is_InitImageQueue (hCam, 0);                             //-->deprecated
  //nRet=is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_INIT, NULL, 0);  
  //if(nRet == IS_SUCCESS){
    // we got buffers in the image queue
  //  std::cout<<"Enabling/Initialization of image_queue was successful"<<std::endl;
  //}
  nRet = is_InitImageQueue (hCam, 0);                 //substitute is_ImageQueue(init)
  
}



void Ueye_camera::apply_automatic_focus(int afmin,int afmax){

  UINT supported_sharpness_calculation_algorithm;
  //Returns the algorithms for sharpness calculation of the autofocus (AUTOFOCUS_SHARPNESS_CALCULATION_ALGORITHM) supported by the camera.
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_SUPPORTED_SHARPNESS_CALCULATION_ALGORITHM, &supported_sharpness_calculation_algorithm, sizeof(supported_sharpness_calculation_algorithm));
 
  if (IS_SUCCESS == nRet)
  {
    UINT sharpness_calculation_algorithm;

    is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_SHARPNESS_CALCULATION_ALGORITHM_DEFAULT, &sharpness_calculation_algorithm, sizeof(sharpness_calculation_algorithm));

    //Sets the algorithm for sharpness calculation of the autofocus.
    nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_SHARPNESS_CALCULATION_ALGORITHM, &sharpness_calculation_algorithm, sizeof(sharpness_calculation_algorithm));
    if (IS_SUCCESS == nRet)
    {
      sharpness_calculation_algorithm = 0;
      //Returns the set algorithm for sharpness calculation of the autofocus.
      nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_SHARPNESS_CALCULATION_ALGORITHM, &sharpness_calculation_algorithm, sizeof(sharpness_calculation_algorithm));
 
      if (IS_SUCCESS == nRet)
      {
          std::cout<<"The sharpness algorithm has been set properly"<<std::endl;
      }
    }
  }
//#######################################################################################################################  Focus - Search Algorithm

  UINT supported_search_peak_algorithm;
  // Returns the peak search algorithms
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_ONCE_SUPPORTED_PEAK_SEARCH_ALGORITHM, &supported_search_peak_algorithm, sizeof(supported_search_peak_algorithm));

  if (IS_SUCCESS == nRet)
  {
    UINT search_peak_algorithm;
    //if (AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_HILL_CLIMBING == (supported_search_peak_algorithm & AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_HILL_CLIMBING_SEARCH))
    //{
    //  search_peak_algorithm = AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_HILL_CLIMBING_SEARCH;
    //}
    //else
    //{
      //Returns the default peak search algorithm for the autofocus.
      is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_DEFAULT, &search_peak_algorithm, sizeof(search_peak_algorithm));
    //}
    //the same PRINT to check ON HOW THIS THING WORKS...
    //(checked)std::cout<<"The autofocus algorithm set is :"<<search_peak_algorithm<<"\t AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_GOLDEN_RATIO_SEARCH: "<<AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_GOLDEN_RATIO_SEARCH<< "\tAUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_GLOBAL_SEARCH: "<<AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM_GLOBAL_SEARCH<<std::endl;

    //Sets the peak search algorithm for the autofocus.
    nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM, &search_peak_algorithm, sizeof(search_peak_algorithm));
    if (IS_SUCCESS == nRet)
    {
      search_peak_algorithm = 0;
      //Returns the set peak search algorithm for the autofocus.
      nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_ONCE_PEAK_SEARCH_ALGORITHM, &search_peak_algorithm, sizeof(search_peak_algorithm));

      if (IS_SUCCESS == nRet)
      {
          std::cout<<"The autofocus algorithm has been set properly"<<std::endl;
      }
    }
  }
//######################################################################################################################################  Focus - AOI
  UINT number_of_supported_focus_aois;
  //check whether AOIS are supported and if yes, how many?
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_NUMBER_OF_SUPPORTED_AOIS, &number_of_supported_focus_aois, sizeof(number_of_supported_focus_aois));
  
  if ((IS_SUCCESS == nRet) && (0 < number_of_supported_focus_aois))
  {
    IS_SIZE_2D focus_aoi_min_size;
    //check the minimum size for setting an AOI...
    nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_AOI_SIZE_MIN, &focus_aoi_min_size, sizeof(focus_aoi_min_size)); 
    //set the focus_aoi variable..
    AUTOFOCUS_AOI focus_aoi;  //AUTOFOCUS_AOI structure
    focus_aoi.uNumberAOI = 1;
    focus_aoi.rcAOI.s32X = 200;
    focus_aoi.rcAOI.s32Y = 200;
    focus_aoi.rcAOI.s32Width = focus_aoi_min_size.s32Width;
    focus_aoi.rcAOI.s32Height = focus_aoi_min_size.s32Height;
    focus_aoi.eWeight = AUTOFOCUS_AOI_WEIGHT_MIDDLE;
    //set the AOI of the whole frame 
    nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_AOI, &focus_aoi, sizeof(focus_aoi));
    if (IS_SUCCESS == nRet)
    {
      memset(&focus_aoi, 0, sizeof(focus_aoi)); //Initialize AOI - sets the 2D array of zeros
      //get the AOI of the whole frame - so as to define an input for the Autofocus algorithm
      nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_AOI, &focus_aoi, sizeof(focus_aoi));
      if (IS_SUCCESS == nRet)
      {
          std::cout<<"Focus AOI has been set : "<<std::endl;//<<focus_aoi<<std::endl;
      }
  }
  AUTOFOCUS_AOI_PRESET aoi_preset = AUTOFOCUS_AOI_PRESET_CENTER;
  
  //Sets a predefined set (AUTOFOCUS_AOI_PRESET) of supported focus AOI. -- > That is the center of the frame --> we should be able to define a new area according to the localization of the captured context
  //nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_AOI_PRESET, &aoi_preset, sizeof(aoi_preset));
  //if (IS_SUCCESS == nRet)
  //{
  //    AUTOFOCUS_AOI focus_aoi_preset;
      //Returns the set focus AOI for the autofocus.
  //    nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_AOI, &focus_aoi_preset, sizeof(focus_aoi_preset));
 
  //    if (IS_SUCCESS == nRet)
  //    {
  //        std::cout<<"Focus AOI-preset has been set : "<<std::endl;//focus_aoi_preset<<std::endl;
  //    }
  //}
}
//#######################################################################################################  Focus - Autofocus limit for the focus search range
  AUTOFOCUS_LIMIT autofocus_limit_default;
  //Returns the minimum and maximum limit for the default focus search range.
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_LIMIT_DEFAULT, &autofocus_limit_default, sizeof(autofocus_limit_default));
 
  if (IS_SUCCESS == nRet)
  {
    //Set the AUTOFOCUS_LIMIT structure
    AUTOFOCUS_LIMIT autofocus_limit;
    autofocus_limit.sMin = afmin;//300;//-1;
    autofocus_limit.sMax = afmax;//380;//500;
    //Sets the minimum and maximum limit for the focus search range.
    nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_LIMIT, &autofocus_limit, sizeof(autofocus_limit));
   
  if (IS_SUCCESS == nRet)
  {
      autofocus_limit.sMin = 0;
      autofocus_limit.sMax = 0;
      //Returns the minimum and maximum limit for the focus search range.
      nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_LIMIT, &autofocus_limit, sizeof(autofocus_limit));
      std::cout<<"Autofocus limit : MAX="<<autofocus_limit.sMax<<"\t MIN="<<autofocus_limit.sMin<<std::endl;

      if (IS_SUCCESS == nRet)
      {
        std::cout<<"Autofocus limit is set "<<std::endl;
        //nRet = is_Focus (hCam, FOC_CMD_SET_ENABLE_AUTOFOCUS, NULL, 0);
      }
  }
}

//#######################################################################################################  Focus - response time of the lens 
  WORD autofocus_lens_response_time_default;
  //Returns the default response time of the lens (positioning time) in milliseconds.
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_LENS_RESPONSE_TIME_DEFAULT, &autofocus_lens_response_time_default, sizeof(autofocus_lens_response_time_default));
  
  if (IS_SUCCESS == nRet)
  {
    //Sets the response time of the lens (positioning time) in milliseconds.
    nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_LENS_RESPONSE_TIME, &autofocus_lens_response_time_default, sizeof(autofocus_lens_response_time_default));
   
    if (IS_SUCCESS == nRet)
    {
      WORD autofocus_lens_response_time;
      //Returns the response time of the lens (positioning time) in milliseconds.
      nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_LENS_RESPONSE_TIME, &autofocus_lens_response_time, sizeof(autofocus_lens_response_time));
      std::cout<<"autofocus_lens_response_time : "<<autofocus_lens_response_time<<std::endl;
      if (IS_SUCCESS == nRet)
      {
          std::cout<<"Autofocus - response time of the lens has been set properly"<<std::endl;
      }
    }
  }
//#######################################################################################################  Focus - hysteresis 
                        //(hysteresis) Defines the size of the interval range after which the peak search algorithm terminates the search.
  WORD autofocus_hysteresis_default;
  //Returns the default hysteresis value ("search accuracy").
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_HYSTERESIS_DEFAULT, &autofocus_hysteresis_default, sizeof(autofocus_hysteresis_default));
 
  if (IS_SUCCESS == nRet)
  {
    //Sets the hysteresis value ("search accuracy").
    nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_HYSTERESIS, &autofocus_hysteresis_default, sizeof(autofocus_hysteresis_default));
   
    if (IS_SUCCESS == nRet)
    {
      WORD autofocus_hysteresis;
      //Returns the hysteresis value ("search accuracy").
      nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_HYSTERESIS, &autofocus_hysteresis, sizeof(autofocus_hysteresis));
 
      if (IS_SUCCESS == nRet)
      {
          std::cout<<"Autofocus - hysteresis has been set properly"<<std::endl;
      }
    }
  }
//#######################################################################################################  Focus - callback sharpness
 
  AUTOFOCUS_CALLBACK autofocus_callback;
  autofocus_callback.pfFunc = print_focus_sharpness;
  autofocus_callback.pContext = NULL;
 
  nRet = is_Focus(hCam, FOC_CMD_SET_AUTOFOCUS_CALLBACK, &autofocus_callback, sizeof(autofocus_callback));
 
  if (IS_SUCCESS == nRet)
  {
    std::cout<<"Defining focus values according to the sharpness of the Focus-AOI"<<std::endl;
    is_Focus(hCam, FOC_CMD_SET_ENABLE_AUTOFOCUS_ONCE, NULL, 0);
  }
//#######################################################################################################  Focus - enable and trigger

  // enable autofocus
  nRet = is_Focus (hCam, FOC_CMD_SET_ENABLE_AUTOFOCUS, NULL, 0);
  // Trigger autofocus once
  is_Focus(hCam, FOC_CMD_SET_ENABLE_AUTOFOCUS_ONCE, NULL, 0);
 
  //Get autofocus status
  INT nValue = 0;
  nRet = is_Focus(hCam, FOC_CMD_GET_AUTOFOCUS_STATUS, (void*)&nValue, sizeof(nValue));
  if (nValue == FOC_STATUS_FOCUSED)
  {
    std::cout<<"Autofocus : FOC_CMD_GET_AUTOFOCUS_STATUS returned FOC_STATUS_FOCUSED "<<std::endl;
  }

//nRet = is_Focus (hCam, FOC_CMD_GET_AUTOFOCUS_ENABLE, enabled_autof, 4);
  //std::cout<<"Enabled autofocus : "<<enabled_autof<<"\t"<<typeid(enabled_autof).name() <<std::endl;

}



void Ueye_camera::disable_automatic_image_control(){
  double execonce=0;
  //Enables/disables the automatic disable for automatic brightness control (gain and exposure time)
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_BRIGHTNESS_ONCE, &execonce, 0);
  method_report(nRet, "automatic_image_control::IS_SET_AUTO_BRIGHTNESS_ONCE");
  //Returns the automatic disable status
  
  nRet = is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &execonce, 0);
}


void Ueye_camera::save_parameters(std::string strfilename){
 
  const size_t len = strfilename.length() + 1;
  wchar_t wfname[len];
  swprintf(wfname, len, L"%s", strfilename.c_str());

  nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_SAVE_FILE, wfname, NULL);

  /*
  // Get the number of supported parameter sets in the camera's user memory
  UINT nNumber;
  INT nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_GET_NUMBER_SUPPORTED, (void*)
                          &nNumber, sizeof(nNumber));
  std::cout<<"####################################### Number of supported params are in total: "<<nNumber<<std::endl;
  std::cout<<"https://en.ids-imaging.com/manuals/ids-software-suite/ueye-manual/4.95/en/sdk_allgemeines_ini-datei.html"<<std::endl;
  */
}


void Ueye_camera::load_parameters(std::string strfilename){
  //wchar_t filename
   
  const size_t len = strfilename.length() + 1;
  wchar_t wfname[len];
  swprintf(wfname, len, L"%s", strfilename.c_str());
  nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_LOAD_FILE, wfname, NULL);

}

void Ueye_camera::apply_automatic_image_control(){
  
  //auto_sexposure_gainc();
  
  auto_white_balance();

}

void Ueye_camera::apply_automatic_image_control(double max_gain){
  
  auto_sexposure_gainc(max_gain);
  
  auto_white_balance();

}

void Ueye_camera::auto_white_balance(){


  double dEnable = 1;
  nRet = is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &dEnable, 0);
  double awb_enabled;
  awb_enabled=1;
  nRet = is_SetAutoParameter (hCam, IS_GET_ENABLE_AUTO_WHITEBALANCE, &awb_enabled, 0);
  method_report(nRet, "auto_white_balance::IS_GET_ENABLE_AUTO_WHITEBALANCE");
  if (awb_enabled){
    std::cout<<"Automatic White Balance is enabled"<<std::endl; 
  }

  // (3) Auto white balance (AWB)
  //Returns the offset values for the red and blue channels.
  double redoffset,blueoffset;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_WB_OFFSET, &redoffset, &blueoffset);
  std::cout<<"AWB::redoffset has default value:"<<redoffset<<std::endl;
  std::cout<<"AWB::blueoffset has default value:"<<blueoffset<<std::endl;
  double redperm,blueperm;
  //Returns the color gain limits for auto white balance. 
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_WB_GAIN_RANGE, &redperm, &blueperm);
  std::cout<<"AWB::The permissible range for the auto white balance::color gain limits, varies from "<<redperm<<" to "<<blueperm<<std::endl;

  //Returns the automatic disable status
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_WB_ONCE, &awb_enabled, 0);
  if (awb_enabled){
    std::cout<<"AWB:: Auto White Balance is already enabled"<<std::endl;
  }
  else if (!awb_enabled){
    std::cout<<"AWB:: Auto White Balance is currently disabled"<<std::endl;
    //Sets automatic disabling of auto white balance
    awb_enabled=1;
    nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_WB_ONCE, &awb_enabled, 0);
    method_report(nRet, "automatic_image_control::IS_SET_AUTO_WB_ONCE");
    
    if (awb_enabled){
    std::cout<<"AWB:: Auto White Balance (ONCE) is now enabled ... "<<std::endl;
    std::cout<<" ... and redoffset blueoffset values are currently the following :"<<redoffset<<", "<<blueoffset<<std::endl;
    }
    else if (!awb_enabled) std::cout<<"AWB:: Auto White Balance (ONCE) COULD NOT BE ENABLED"<<std::endl;
  }
}


void Ueye_camera::auto_sexposure_gainc(double max_gain){
  //IS_SET_ENABLE_AUTO_SENSOR_SHUTTER,IS_SET_ENABLE_AUTO_SENSOR_GAIN,IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE  are not supported...
  //Enable auto exposure shutter(AES), auto gain control(AGC), white balance function(AWB)
  double dEnable = 1;
  nRet = is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_SHUTTER, &dEnable, 0);
  nRet = is_SetAutoParameter (hCam, IS_SET_ENABLE_AUTO_GAIN, &dEnable, 0);
  double aes_enabled,agc_enabled;
  aes_enabled=1;
  agc_enabled=1; 

  nRet = is_SetAutoParameter (hCam, IS_GET_ENABLE_AUTO_SHUTTER, &aes_enabled, 0);
  method_report(nRet, "auto_sexposure_gainc::IS_GET_ENABLE_AUTO_SHUTTER");
  nRet = is_SetAutoParameter (hCam, IS_GET_ENABLE_AUTO_GAIN, &agc_enabled, 0);
  method_report(nRet, "auto_sexposure_gainc::IS_GET_ENABLE_AUTO_GAIN");
  
  if (aes_enabled && agc_enabled){
    std::cout<<"Auto Exposure Shutter AND Auto Gain Control are enabled"<<std::endl; 
  }

  //defines the setpoint (average brightness of the image); the following rule applies independently of the image bit depth:
  //0 = black
  //128 = 50% gray (default)
  //255 = white
  double nominal = 128; 
  //Sets the setpoint for auto gain control/auto exposure shutter. 
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_REFERENCE, &nominal, 0);
  method_report(nRet, "auto_sexposure_gainc::IS_SET_AUTO_REFERENCE");
  double setpoint;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_REFERENCE, &setpoint, 0);
  std::cout<<"AES;AGC::Brightness setpoint is set to the value of "<<setpoint<<std::endl;
  
  //Returns the upper limit for auto gain control.   
  double maxGain;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_GAIN_MAX, &maxGain, 0);
  std::cout<<"AGC::upper limit for auto exposure shutter: "<<maxGain<<std::endl;
  maxGain=static_cast<float>(max_gain);
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_GAIN_MAX, &maxGain, 0);
  method_report(nRet, "auto_sexposure_gainc::IS_SET_AUTO_GAIN_MAX");
    std::cout<<"AGC::Max gain for the auto-gain has been set"<<std::endl;
  double maxShutter;
  //Returns the upper limit for auto exposure shutter. 
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_SHUTTER_MAX, &maxShutter, 0);
    std::cout<<"AES::Max shutter="<<maxShutter<<std::endl;
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_SHUTTER_MAX, &maxShutter, 0);
  method_report(nRet, "auto_sexposure_gainc::IS_SET_AUTO_SHUTTER_MAX");
    std::cout<<"AES::Max shutter for the auto-exposure has been set"<<std::endl;
  
  double execonce;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_BRIGHTNESS_ONCE, &execonce, 0);
  std::cout<<"EXECUTE ONCE has value : "<<execonce<<std::endl;
  //enable
  execonce=1;
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_BRIGHTNESS_ONCE, &execonce, 0);
  method_report(nRet, "automatic_image_control::IS_SET_AUTO_BRIGHTNESS_ONCE");
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_BRIGHTNESS_ONCE, &execonce, 0);
  if (execonce){
    std::cout<<"EXECUTE ONCE has been enabled"<<std::endl;
  }

  double autospeed;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_SPEED, &autospeed, 0);
  std::cout<<"autospeed param has value : "<<autospeed<<std::endl;
  autospeed=50;
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_SPEED, &autospeed, 0);
  method_report(nRet, "automatic_image_control::IS_SET_AUTO_SPEED");
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_SPEED, &autospeed, 0);
  std::cout<<"autospeed param has value (after changing): "<<autospeed<<std::endl;

  double skipframes;
  //Returns the number of frames to be skipped during automatic control.
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_SKIPFRAMES, &skipframes, 0);
  std::cout<<"auto skipped frames are="<<skipframes<<std::endl;
  //Returns the permissible range for the number of frames to be skipped.
  double sfrange_min,sfrange_max;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_SKIPFRAMES_RANGE, &sfrange_min, &sfrange_max);
  std::cout<<"The permissible range for the number of frames to be skipped varies from "<<sfrange_min<<" to "<<sfrange_max<<std::endl;

  //Returns the hysteresis value for auto exposure shutter and auto gain control. 
  double ahyster;
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_HYSTERESIS, &ahyster, 0);
  std::cout<<"Histerisis values (default) is :"<<ahyster<<std::endl;
  nRet = is_SetAutoParameter (hCam, IS_SET_AUTO_HYSTERESIS, &ahyster, 0);
  method_report(nRet, "automatic_image_control::IS_SET_AUTO_HYSTERESIS");
  if (nRet) std::cout<<"AIC::Histerisis value is set with :"<<ahyster<<std::endl;
  double hstrange_min,hstrange_max;
  //Returns the permissible range for the hysteresis value.
  nRet = is_SetAutoParameter (hCam, IS_GET_AUTO_HYSTERESIS_RANGE, &hstrange_min, &hstrange_max);
  std::cout<<"The permissible range for the hysteresis value varies from "<<hstrange_min<<" to "<<hstrange_max<<std::endl;

//Photometric settings for auto gain control/auto exposure shutter
  double aesphotom,default_aesphotom;
  //Returns the photometry mode for auto exposure shutter. 
  nRet = is_SetAutoParameter (hCam, IS_GET_SENS_AUTO_SHUTTER_PHOTOM, &aesphotom, 0);
  std::cout<<"AES::Photometry mode is set as "<<aesphotom<<std::endl;
  // Returns the default photometry mode for auto exposure shutter. 
  nRet = is_SetAutoParameter (hCam, IS_GET_SENS_AUTO_SHUTTER_PHOTOM_DEF, &default_aesphotom, 0);
  std::cout<<"AES::The Default Photometry mode is "<<default_aesphotom<<std::endl;
  // Returns the photometry mode for auto gain control. 
  double agcphotom;
  nRet = is_SetAutoParameter (hCam, IS_GET_SENS_AUTO_GAIN_PHOTOM, &agcphotom, 0);
  std::cout<<"AGC::The Photometry mode for auto gain control is "<<agcphotom<<std::endl;
}

void Ueye_camera::set_parameters(double fps_requested){

  double newDisplaySizeX=DisplayWidth;          // (double) s.resolution.at(0);
  double newDisplaySizeY= DisplayHeight;        //(double) s.resolution.at(1);
  set_display_size(newDisplaySizeX,newDisplaySizeY);

  //Set color mode
  if (sInfo.nColorMode==IS_COLORMODE_BAYER){
    nColorMode=IS_CM_BGR8_PACKED;
    nBitsPerPixel=24;
  }
  else if (sInfo.nColorMode==IS_COLORMODE_CBYCRY)
  {
    nColorMode=IS_CM_BGRA8_PACKED;
    nBitsPerPixel=32;
  }
  else
  {
    nColorMode=IS_CM_MONO8;
    nBitsPerPixel=8;
  }
  nRet=is_SetColorMode(hCam,nColorMode);
  method_report(nRet,"set_parameters::is_SetColorMode(hCam,nColorMode)");
  std::cout<<"ColorMode:\t\t"<<nColorMode<<std::endl;
  std::cout<<"BitsPerPixel:\t\t"<<nBitsPerPixel<<std::endl;

  //Set display mode;
  nRet=is_SetDisplayMode(hCam,IS_SET_DM_DIB);
  method_report(nRet,"set_parameters::is_SetDisplayMode(hCam,IS_SET_DM_DIB)");

  /*
  UINT nPixelClock;
  // Get current pixel clock
  nRet = is_PixelClock(hCam, IS_PIXELCLOCK_CMD_GET, (void*)&nPixelClock, sizeof(nPixelClock));
  std::cout<<"nPixelClock "<<nPixelClock<<std::endl;
  
  UINT nRange[3];
  ZeroMemory(nRange, sizeof(nRange));
  // Get pixel clock range
  INT nRet = is_PixelClock(hCam, IS_PIXELCLOCK_CMD_GET_RANGE, (void*)nRange, sizeof(nRange));
  if (nRet == IS_SUCCESS)
  {
  UINT nMin = nRange[0];
  UINT nMax = nRange[1];
  UINT nInc = nRange[2];
  std::cout<<"PIXEL CLOCK nMin:"<<nMin<<", nMax:"<<nMax<<", nInc:"<<nInc<<std::endl;
  }
  */

  //Set FrameRate
  std::cout<<"fps_requested "<<fps_requested<<std::endl;
  nRet=is_SetFrameRate (hCam,fps_requested, &actualfps);
  method_report(nRet,"set_parameters::is_SetFrameRate (hCam,fps_requested, &actualfps)");
  std::cout<<"actualfps "<<actualfps<<std::endl;
  fps=actualfps;
  std::cout<<"Fps are set to : "<<fps<<std::endl;
}

void Ueye_camera::set_display_size(int X,int Y){
  this->DisplayWidth=X;
  this->DisplayHeight=Y;
}

void Ueye_camera::camera_info(){
  std::cout<<"Retrieving information about the cameras connected"<<std::endl;
  INT nNumCam;
  if(is_GetNumberOfCameras( &nNumCam) == IS_SUCCESS) {
    if( nNumCam >= 1 ) {
      // Create new list with suitable size
      //UEYE_CAMERA_LIST* pucl;
      pucl = (UEYE_CAMERA_LIST*) new BYTE [sizeof (DWORD) + nNumCam * sizeof (UEYE_CAMERA_INFO)];
      pucl->dwCount = nNumCam;
      //Retrieve camera info
      if (is_GetCameraList(pucl) == IS_SUCCESS) {
        int iCamera;
        for (iCamera = 0; iCamera < (int)pucl->dwCount; iCamera++) {
          //Test output of camera info on the screen
          printf("Camera %i Id: %d\n", iCamera,
          pucl->uci[iCamera].dwCameraID);
        }
      }
      delete [] pucl;
    }
  }

  //get_sensor_info();
  std::cout<<"Collecting information about the camera sensor"<<std::endl;
  
  nRet=is_GetSensorInfo(hCam,&sInfo);
  this->method_report(nRet,"camera_info::is_GetSensorInfo(hCam,&sInfo)");
  std::cout<<"Cameramodel:\t\t"<<sInfo.strSensorName<<std::endl;
  std::cout<<"Maximum image width:\t\t"<<sInfo.nMaxWidth<<std::endl;
  std::cout<<"Maximum image height:\t\t"<<sInfo.nMaxHeight<<std::endl;
  // std::cout<<"Selected image width:\t\t"<<s.resolution.at(0)<<std::endl;
  // std::cout<<"Selected image height:\t\t"<<s.resolution.at(1)<<std::endl;

  MaxImageSizeX=sInfo.nMaxWidth;
  MaxImageSizeY=sInfo.nMaxHeight;

  std::cout<<"Collecting information about the camera type"<<std::endl;
  CAMINFO CamInfo;
  nRet=is_GetCameraInfo(hCam, &CamInfo);
  method_report(nRet,"camera_info::is_GetCameraInfo(hCam, &CamInfo)");
  //std::cout<<"CamInfo"<<CamInfo<<std::endl;
}


void Ueye_camera::get_cameras_connected(){
    int num_cameras;
    if (is_GetNumberOfCameras(&num_cameras)==IS_SUCCESS){
        if (num_cameras >= 1){
            
            pucl=(UEYE_CAMERA_LIST*) new BYTE [sizeof (DWORD) + num_cameras*sizeof(UEYE_CAMERA_INFO)];
            pucl->dwCount=num_cameras;
        
    

            if (is_GetCameraList(pucl)==IS_SUCCESS){
                int iCamera;
                for (iCamera=0;iCamera<(int)pucl->dwCount;iCamera++){
                    pucl->uci[iCamera].dwCameraID=iCamera+1;
                    printf("Camera %i Id: %d\n",iCamera,pucl->uci[iCamera].dwCameraID);
                }
            }
            //delete [] pucl;
        }
    }
}

Ueye_camera::Ueye_camera(int id,int testing){
  int num_cameras;
    if (is_GetNumberOfCameras(&num_cameras)==IS_SUCCESS){
        if (num_cameras >= 1){
            
            pucl=(UEYE_CAMERA_LIST*) new BYTE [sizeof (DWORD) + num_cameras*sizeof(UEYE_CAMERA_INFO)];
            pucl->dwCount=num_cameras;
        
    

            if (is_GetCameraList(pucl)==IS_SUCCESS){
                int iCamera;
                for (iCamera=0;iCamera<(int)pucl->dwCount;iCamera++){
                    pucl->uci[iCamera].dwCameraID=iCamera+1;
                    printf("Camera %i Id: %d\n",iCamera,pucl->uci[iCamera].dwCameraID);

                    printf("Camera %i Id, DeviceID: %d\n",iCamera,pucl->uci[iCamera].dwDeviceID);
                }
            }
            //delete [] pucl;
        }
    }

  for (int iCam=0;iCam<(int)pucl->dwCount;iCam++){
    if (id==pucl->uci[iCam].dwCameraID){
      this->camera_id=id;
    }
  }
  //hCam=pucl->uci[id].dwDeviceID | IS_USE_DEVICE_ID;
  hCam=id ;
  hWndDisplay=NULL;
  nRet=is_InitCamera(&hCam,hWndDisplay);
  this->method_report(nRet,"Ueye_camera(int id)");
}

Ueye_camera::Ueye_camera(Settings &s){
  this->s=s;
  this->camera_id=s.camera_id;
  hCam=camera_id | IS_USE_DEVICE_ID;
  hWndDisplay=NULL;
  nRet=is_InitCamera(&hCam,hWndDisplay);
  this->method_report(nRet,"Ueye_camera(int id)");
}

Ueye_camera::Ueye_camera(){
  this->camera_id=0;
  hCam=camera_id;
  hWndDisplay=NULL;
  nRet=is_InitCamera(&hCam,hWndDisplay);
  this->method_report(nRet,"Ueye_camera(void)");
}

Ueye_camera::~Ueye_camera(){
  nRet = is_ExitCamera(hCam);
  method_report(nRet,"~Ueye_camera(void)");
}