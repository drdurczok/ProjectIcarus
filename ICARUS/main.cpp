#include "include/Eyes.h"
#include "include/CalibrationEye.h"

int main( int argc, const char** argv)
{
    bool dualCameras = true;
    bool saveData = true;

    CalibrationEye calib;
    calib.calibration(1,saveData,dualCameras,100);
    //calib.calibrationFromFiles(2);

    Eyes sight;
    //sight.undistortImg();
    //sight.recordVideo();
    //sight.depthMap();
    return 0;
}




