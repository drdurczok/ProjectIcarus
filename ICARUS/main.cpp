#include "include/Eyes.h"
#include "include/CalibrationEye.h"

int main( int argc, const char** argv)
{
    bool dualCameras = true;
    bool saveData = true;

    CalibrationEye calib;
    calib.calibration(saveData,dualCameras);

    Eyes sight;
    //sight.undistortImg();
    //sight.recordVideo();
    //sight.depthMap();
    return 0;
}




