#include "include/Eyes.h"
#include "include/CalibrationEye.h"

int main( int argc, const char** argv)
{
    bool dualCameras = true;
    bool saveData = true;

    //CalibrationEye calib;
    //calib.calibration(saveData,dualCameras);
    Eyes sight;

    Mat distanceCoefficients;
    Mat cameraMatrix;
    sight.loadCameraCalibration("CameraCalibration", cameraMatrix, distanceCoefficients);
    cout << cameraMatrix << endl;
    cout << distanceCoefficients << endl;

    Mat img = imread("image.jpg");
    Mat undistorted;
    undistort(img, undistorted, cameraMatrix, distanceCoefficients);
    imwrite("calibresult.jpg",undistorted);
    //sight.recordVideo();
    //sight.depthMap();
    return 0;
}




