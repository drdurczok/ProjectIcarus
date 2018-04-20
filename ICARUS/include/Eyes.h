#ifndef EYES_H
#define EYES_H

#include "EyeBase.h"

class Eyes: public EyeBase
{
    public:
        Eyes();
        Eyes(unsigned, unsigned);
        virtual ~Eyes();
        void swapCameras();
        unsigned getCameraState(unsigned);
        unsigned dispCamera(int);
        unsigned dispUndistImage();
        unsigned dispRectImage();
        unsigned calibration(unsigned, bool, bool, unsigned);
        unsigned stereoCalibration(unsigned);	//stereoCalibrate, stereoRectify


    private:
    	
};

#endif // EYES_H
