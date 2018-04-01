#ifndef CALIBRATIONEYE_H
#define CALIBRATIONEYE_H

#include "EyeBase.h"

class CalibrationEye: public EyeBase
{
    public:
        CalibrationEye();
        unsigned dispCamera(unsigned);
        unsigned dispRectImage();
        unsigned calibration(unsigned,unsigned,bool,unsigned);
        unsigned calibrationFromFiles(unsigned);
        unsigned stereoCalibration(int);
        void swapCameras();
        unsigned checkFolder(unsigned,unsigned);
        bool rmFolder(unsigned);


    private:
        void cameraCalibration(vector<Mat>, Size, float, Mat&, Mat&);
        void createKnownBoardPosition(Size, float, vector<Point3f>&);
        void getChessboardCorners(vector<Mat>, vector<vector<Point2f>>&, bool);

        const float calibrationSquareDimension = 0.025f; //meters
        const unsigned board_height = 6;
        const unsigned board_width = 9;
        const Size chessboardDimensions = Size(board_height, board_width);

        camera cam[3];
};

#endif // CALIBRATIONEYE_H
