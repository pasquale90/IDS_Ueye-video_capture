# Tested on 
## environment : Ubuntu 20.04 focal, Ubuntu 22.04 LTS jammy
## g++ version : g++ 10.4.0
## ueye model : UI-3884LE-C-HQ

# to build and run
$ make all
$ ./bin/record_video

# to clean files
$ make clean

# Pseudo code description

    1. Initialize the camera (is_InitCamera).
    2. Set the camera parameters.
    3. Allocate the buffers in a loop:
        Allocate the image memory (is_AllocImageMem).
        Add the image memory to the sequence (is_AddToSequence).
    4. Initialize image queue (is_InitImageQueue).
    5. Start video capture (is_CaptureVideo).
    6. Acquire the images from camera in a loop:
        Wait for the next image (is_WaitForNextImage). If it was a success proceed, else check the capture status and handle the errors.
        Copy the current frame (is_CopyImageMem) with the buffer pointer and memory id returned by previous function for image processing (in this case saving the image to file). The image processing should be done in a separate thread in order to avoid blocking this one (but this can be ignored if image processing part is faster than the acquisition).
        Unlock the current buffer in sequence (with the buffer pointer and memory id defined previously) so the next image acquisition can start.
    7. After the end of the image acquisition loop stop the video capture (is_StopLiveVideo).
    8. Exit the image queue (is_ExitImageQueue).
    9. Clear the buffer sequence (is_ClearSequence).
    10. Deallocate the buffers in a loop (is_FreeImageMem).
    11. Exit camera (is_ExitCamera

