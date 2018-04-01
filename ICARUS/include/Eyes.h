#ifndef EYES_H
#define EYES_H

#include "EyeBase.h"

class Eyes: public EyeBase
{
    public:
        Eyes();
        virtual ~Eyes();
        void swapCameras();
        unsigned dispCamera(unsigned);
        unsigned dispRectImage();
        unsigned calibration(unsigned, bool, bool, unsigned);
        unsigned stereoCalibration(unsigned);

    private:
    	VideoCapture *vid[3];
};

#endif // EYES_H
