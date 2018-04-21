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
        virtual ~EyeBase();
        unsigned calibrationFromFiles(unsigned, unsigned, unsigned);
        unsigned checkFolder(unsigned,unsigned,unsigned);
        bool rmFolder(unsigned);
        void swapCameras();

    protected:
        void initializeCamera(unsigned i[], unsigned);
        Mat getFrame(unsigned);
        void formatData(unsigned);

        double cameraCalibration(vector<Mat>, Mat&, Mat&);    //calibrateCamera, returns RMS(root mean square) - good calibration between 0.1 and 1.0 pixels
        void createKnownBoardPosition(vector<Point3f>&);
        bool getChessboardCorners(vector<Mat>, vector<vector<Point2f>>&, bool);

        vector<Point3f> Create3DChessboardCorners(Size, float);
        void createRMap();


        float squareEdgeLength; //meters
        unsigned board_height;
        unsigned board_width;
        Size boardSize;

        struct camera{
                // Index and documentation values
                unsigned num;
                unsigned vid;
                unsigned posx, posy;
                string title;
                // Frame manipulation data
                double angle;
                Mat rotate;
                Rect bbox;
                Rect ROI;
                Size size;
                // Calibration parameters
                Mat frame;
                Mat drawToFrame;
                vector<Mat> savedImages;
                int i=1;
        };
        camera cam[3];

        VideoCapture *vid[3];

    private:

};

#endif // EYEBASE_H
