#ifndef EYES_H
#define EYES_H

#include "EyeBase.h"

class Eyes: public EyeBase
{
    public:
        Eyes();
        Eyes(unsigned, unsigned);
        virtual ~Eyes();
        void initializeCamera();
        void initializeCamera(char);
        void swapCameras();
        unsigned getCameraState(unsigned);
        unsigned dispCamera(unsigned);
        unsigned dispUndistImage();
        unsigned dispRectImage();
        unsigned calibration(unsigned, bool, bool, unsigned);
        unsigned stereoCalibration(unsigned);	//stereoCalibrate, stereoRectify


    private:
    	VideoCapture *vid[3];
    	unsigned camVid[2];
};

#endif // EYES_H
