#ifndef CALIBRATIONEYE_H
#define CALIBRATIONEYE_H

#include "EyeBase.h"

class CalibrationEye: public EyeBase
{
    public:
        CalibrationEye();
        unsigned calibration(bool,bool);

    private:
        void cameraCalibration(vector<Mat>, Size, float, Mat&, Mat&);
        void createKnownBoardPosition(Size, float, vector<Point3f>&);
        void getChessboardCorners(vector<Mat>, vector<vector<Point2f>>&, bool);

        const float calibrationSquareDimension = 0.026f; //meters
        const Size chessboardDimensions = Size(6,9);
};

#endif // CALIBRATIONEYE_H
