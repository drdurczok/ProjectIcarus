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
        void getCameraState(unsigned*);
        unsigned dispCamera(unsigned);
        unsigned dispRectImage();
        unsigned calibration(unsigned, bool, bool, unsigned);
        unsigned stereoCalibration(unsigned);


    private:
    	VideoCapture *vid[3];
    	unsigned camVid[2];
};

#endif // EYES_H
