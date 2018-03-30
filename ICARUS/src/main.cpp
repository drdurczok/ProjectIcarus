
//#include "../include/Eyes.h"
#include "../include/CalibrationEye.h"
#include "../include/DisparityEye.h"

using namespace std;

int main( int argc, const char** argv)
{
    bool dualCameras = true;
    bool saveData = true;

    CalibrationEye calib;
    //calib.calibration(1,saveData,dualCameras,0);
    calib.calibrationFromFiles(2);
    calib.stereoCalibration(17);

    //Eyes sight;
    //sight.undistortImg();
    //sight.recordVideo();
    //sight.depthMap();
    
    DisparityEye sight;
    sight.depthMap();

    return 0;
}

/*

  printf("Starting Rectification\n");

  cv::Mat R1, R2, P1, P2, Q;
  stereoRectify(K1, D1, K2, D2, img1.size(), R, T, R1, R2, P1, P2, Q);

  fs1 << "R1" << R1;
  fs1 << "R2" << R2;
  fs1 << "P1" << P1;
  fs1 << "P2" << P2;
  fs1 << "Q" << Q;

  printf("Done Rectification\n");
*/