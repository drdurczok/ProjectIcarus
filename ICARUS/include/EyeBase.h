#ifndef EYEBASE_H
#define EYEBASE_H

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstdio>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;

class EyeBase
{
    public:
        EyeBase();

    protected:
        bool loadCameraCalibration(string, Mat&, Mat&);
        bool saveCameraCalibration(string, Mat&, Mat&);
        void createRMap(Mat&, Mat&);

        Mat rmap[2][2];

        struct camera{
                unsigned num;
                unsigned vid;
                Mat frame;
                Mat drawToFrame;
                Mat cameraMatrix = Mat::eye(3,3, CV_64F);
                Mat distanceCoefficients;
                vector<Mat> savedImages;
                ostringstream name;
                ostringstream nameOverlay;
                string title;
                int i=1;
        };

    private:

};

#endif // EYEBASE_H
