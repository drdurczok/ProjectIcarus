#ifndef EYES_H
#define EYES_H

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

class Eyes
{
    public:
        Eyes();
        virtual ~Eyes();
        bool loadCameraCalibration(string, Mat&, Mat&);
        void depthMap();
        void depthMapIMG();
        unsigned recordVideo();

    private:
};

#endif // EYES_H
