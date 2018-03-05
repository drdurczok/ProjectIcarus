#ifndef EYES_H
#define EYES_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <string>
#include <utility>
#include <iostream>

class Eyes
{
    public:
        Eyes();
        virtual ~Eyes();
        void depthMap();
        void depthMapIMG();
        unsigned recordVideo();

    protected:

    private:
};

#endif // EYES_H
