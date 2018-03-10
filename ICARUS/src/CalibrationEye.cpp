#include "CalibrationEye.h"

CalibrationEye::CalibrationEye()
{

}

void CalibrationEye::createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners){
    for(int i=0; i<boardSize.height; i++){
        for(int j=0; j<boardSize.width; j++){
            corners.push_back(Point3f(j*squareEdgeLength, i*squareEdgeLength, 0.0f));
        }
    }
}

void CalibrationEye::getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false){
    for(vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++){
        vector<Point2f> pointBuf;
        bool found = findChessboardCorners(*iter, Size(9,6), pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

        if(found){
            allFoundCorners.push_back(pointBuf);
        }

        if(showResults){
            drawChessboardCorners(*iter, Size(9,6), pointBuf, found);
            imshow("Looking for Corners", *iter);
            waitKey(0);
        }
    }
}

void CalibrationEye::cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float squaredEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients){
    vector<vector<Point2f>> checkerboardImageSpacePoints;

    getChessboardCorners(calibrationImages, checkerboardImageSpacePoints, false);

    vector<vector<Point3f>> worldSpaceCornerPoints(1);

    createKnownBoardPosition(boardSize, squaredEdgeLength, worldSpaceCornerPoints[0]);
    worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(),worldSpaceCornerPoints[0]);

    vector<Mat> rVectors, tVectors;
    distanceCoefficients = Mat::zeros(8,1, CV_64F);

    calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints, boardSize, cameraMatrix, distanceCoefficients, rVectors, tVectors);
}

unsigned CalibrationEye::calibration(bool saveData, bool dual){
    camera cam, cam2;

    VideoCapture vid(1);
    VideoCapture vid2(2);
    if(!vid.isOpened()) return 0;

    namedWindow("Cam", CV_WINDOW_AUTOSIZE);
    if (dual == 1) namedWindow("Cam2", CV_WINDOW_AUTOSIZE);

    int framesPerSecond = 20;

    while(true){
        if(!vid.read(cam.frame)) break;

        vector<Vec2f> foundPoints;
        bool found = false;

        found = findChessboardCorners(cam.frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE); //Use CV_CALIB_FAST_CHECK to increase frames but lower accuracy
        cam.frame.copyTo(cam.drawToFrame);
        drawChessboardCorners(cam.drawToFrame, chessboardDimensions, foundPoints, found);

        if(found) imshow("Cam", cam.drawToFrame);
        else imshow("Cam", cam.frame);

        if (dual == 1) {
            if(!vid2.read(cam2.frame)) break;

            found = findChessboardCorners(cam2.frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE); //Use CV_CALIB_FAST_CHECK to increase frames but lower accuracy
            cam2.frame.copyTo(cam2.drawToFrame);
            drawChessboardCorners(cam2.drawToFrame, chessboardDimensions, foundPoints, found);

            if(found) imshow("Cam2", cam2.drawToFrame);
            else imshow("Cam2", cam2.frame);
        }

        char character = waitKey(1000/framesPerSecond);

        switch(character){
            case ' ':
                //saving image
                if(found){
                    Mat temp;
                    cam.frame.copyTo(temp);
                    cam.savedImages.push_back(temp);
                    if(saveData == 1) {
                        cam.name.str("");
                        cam.nameOverlay.str("");
                        cam.name << "../CalibrationPictures/CAM01_calib" << cam.i << ".jpg";
                        cam.nameOverlay << "../CalibrationPictures/CAM01_calib" << cam.i << "_overlay.jpg";
                        cam.i++;
                        imwrite(cam.name.str(),temp);
                        imwrite(cam.nameOverlay.str(),cam.drawToFrame);
                    }

                    if (dual == 1) {
                        Mat temp2;
                        cam2.frame.copyTo(temp2);
                        cam2.savedImages.push_back(temp2);
                        if(saveData == 1) {
                            cam2.name.str("");
                            cam2.nameOverlay.str("");
                            cam2.name << "../CalibrationPictures/CAM02_calib" << cam2.i << ".jpg";
                            cam2.nameOverlay << "../CalibrationPictures/CAM02_calib" << cam2.i << "_overlay.jpg";
                            cam2.i++;
                            imwrite(cam2.name.str(),temp2);
                            imwrite(cam2.nameOverlay.str(),cam2.drawToFrame);
                        }
                    }
                }
                break;
            case 13:    //enter
                //start calibration
                if(cam.savedImages.size() > 15){
                    cameraCalibration(cam.savedImages, chessboardDimensions, calibrationSquareDimension, cam.cameraMatrix, cam.distanceCoefficients);
                    saveCameraCalibration("CameraCalibration", cam.cameraMatrix, cam.distanceCoefficients);

                     if (dual == 1) {
                        cameraCalibration(cam2.savedImages, chessboardDimensions, calibrationSquareDimension, cam2.cameraMatrix, cam2.distanceCoefficients);
                        saveCameraCalibration("CameraCalibration_2", cam2.cameraMatrix, cam2.distanceCoefficients);
                     }
                }
                break;
            case 27:       //escape
                //exit program
                return 0;
                break;
        }
    }
}
