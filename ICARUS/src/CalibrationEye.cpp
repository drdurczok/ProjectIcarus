#include "CalibrationEye.h"

CalibrationEye::CalibrationEye()
{

}

unsigned CalibrationEye::readSettings(){
    return 0;
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

bool CalibrationEye::saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients){
    ofstream outStream(name);
    if(outStream){
        uint16_t rows = cameraMatrix.rows;
        uint16_t columns = cameraMatrix.cols;

        for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double value = cameraMatrix.at<double>(r,c);
                outStream<<value<<endl;
            }
        }

        rows = distanceCoefficients.rows;
        columns = distanceCoefficients.cols;

         for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double value = distanceCoefficients.at<double>(r,c);
                outStream<<value<<endl;
            }
        }

        outStream.close();
        return true;
    }

    return false;
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

unsigned CalibrationEye::calibration(){
    Mat frame;
    Mat drawToFrame;

    Mat cameraMatrix = Mat::eye(3,3, CV_64F);

    Mat distanceCoefficients;

    vector<Mat> savedImages;

    vector<vector<Point2f>> markerCorners, rejectedCandidates;

    VideoCapture vid(1);

    if(!vid.isOpened()) {return 0;}

    int framesPerSecond = 20;

    namedWindow("Cam", CV_WINDOW_AUTOSIZE);

    while(true){
        if(!vid.read(frame)) break;

        vector<Vec2f> foundPoints;
        bool found = false;

        found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE); //Use CV_CALIB_FAST_CHECK to increase frames but lower accuracy
        frame.copyTo(drawToFrame);
        drawChessboardCorners(drawToFrame, chessboardDimensions, foundPoints, found);

        if(found) imshow("Cam", drawToFrame);
        else imshow("Cam", frame);

        char character = waitKey(1000/framesPerSecond);

        switch(character){
            case ' ':
                //saving image
                if(found){
                    Mat temp;
                    frame.copyTo(temp);
                    savedImages.push_back(temp);
                }
                break;
            case 13:    //enter
                //start calibration
                if(savedImages.size() > 15){
                    cameraCalibration(savedImages, chessboardDimensions, calibrationSquareDimension, cameraMatrix, distanceCoefficients);
                    saveCameraCalibration("CameraCalibration", cameraMatrix, distanceCoefficients);
                }
                break;
            case 27:       //escape
                //exit program
                return 0;
                break;
        }
    }
}

