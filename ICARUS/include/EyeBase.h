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
#include <sys/stat.h>

using namespace std;
using namespace cv;

class EyeBase
{
    public:
        EyeBase();
        unsigned calibrationFromFiles(unsigned, unsigned, unsigned);
        unsigned checkFolder(unsigned,unsigned,unsigned);
        bool rmFolder(unsigned);

    protected:
        void initializeCamera(unsigned i[], unsigned);
        Mat getFrame(unsigned);
        void formatData(unsigned);

        double cameraCalibration(vector<Mat>, Mat&, Mat&);    //calibrateCamera, returns RMS(root mean square) - good calibration between 0.1 and 1.0 pixels
        void createKnownBoardPosition(vector<Point3f>&);
        void getChessboardCorners(vector<Mat>, vector<vector<Point2f>>&, bool);

        void createRMap(Mat&, Mat&);


        float squareEdgeLength; //meters
        unsigned board_height;
        unsigned board_width;
        Size boardSize;

        struct camera{
                unsigned num;
                unsigned vid;
                string title;
                unsigned posx;
                double angle;
                Mat rotate;
                Rect bbox;
                Mat frame;
                Mat drawToFrame;
                Mat cameraMatrix = Mat::eye(3,3, CV_64F);
                Mat distCoefficients;
                vector<Mat> savedImages;
                ostringstream name;
                ostringstream nameOverlay;
                int i=1;
        };
        camera cam[3];

        VideoCapture *vid[3];
        unsigned camVid[2];

    private:

};

#endif // EYEBASE_H
