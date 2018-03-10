#ifndef CALIBRATIONEYE_H
#define CALIBRATIONEYE_H

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class CalibrationEye
{
    public:
        CalibrationEye();
        unsigned readSettings();
        unsigned calibration(bool,bool);

    private:
        void cameraCalibration(vector<Mat>, Size, float, Mat&, Mat&);
        bool saveCameraCalibration(string, Mat, Mat);
        void createKnownBoardPosition(Size, float, vector<Point3f>&);
        void getChessboardCorners(vector<Mat>, vector<vector<Point2f>>&, bool);

        const float calibrationSquareDimension = 0.026f; //meters
        const Size chessboardDimensions = Size(6,9);

        struct camera{
            Mat frame;
            Mat drawToFrame;
            Mat cameraMatrix = Mat::eye(3,3, CV_64F);
            Mat distanceCoefficients;
            vector<Mat> savedImages;
            ostringstream name;
            ostringstream nameOverlay;
            int i=1;
        };
};

#endif // CALIBRATIONEYE_H
