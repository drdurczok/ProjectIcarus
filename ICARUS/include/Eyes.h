#ifndef EYES_H
#define EYES_H

#include "EyeBase.h"

class Eyes: public EyeBase
{
    public:
        Eyes();
        virtual ~Eyes();
        void depthMap();
        void depthMapIMG();
        unsigned recordVideo();
        void undistortImg();

    private:
};

#endif // EYES_H
