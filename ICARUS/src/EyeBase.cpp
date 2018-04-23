#include "../include/EyeBase.h"

EyeBase::EyeBase(){
    squareEdgeLength = 0.026f; //meters
    board_height = 6;
    board_width = 9;
    boardSize = Size(board_height,board_width);

    unsigned offsetx = 30;
    unsigned offsety = 8;

    cam[0].num = 0;
    cam[0].vid = 0;
    cam[0].title = "WebCam";

    cam[1].num = 1;
    cam[1].vid = 1;
    cam[1].title = "LCam";
    cam[1].posx = 20;
    cam[1].angle = 90;
    cam[1].ROI = Rect(1, 1+offsety, 480-offsetx, 480-offsetx);
    cam[1].size = cam[1].ROI.size();

    cam[2].num = 2;
    cam[2].vid = 2;
    cam[2].title = "RCam";
    cam[2].posx = 700;
    cam[2].angle = -90.;
    cam[2].ROI = Rect(1+offsetx, 1, 480-offsetx, 480-offsetx); 
    cam[2].size = cam[2].ROI.size();
}

EyeBase::~EyeBase(){
}

/*
 * This method produces the intrinsic camera parameters from a set of previously saved images.
 * The parameters are stored in an XML file for future reference along with a RMS(root mean square)
 * value that depicts the pixel deviation. A value anywhere from 0.1 to 1.0 is considered a good
 * calibration.
 */

unsigned EyeBase::calibrationFromFiles(unsigned u, unsigned start, unsigned end){
    cam[0].savedImages.clear();
    ostringstream name;
    ostringstream nameOverlay;
    
    for(unsigned i=start; i<end; i++){
        name.str("");
        name << "../../CalibrationPictures/Cam0" << u << "/CAM0" << u << "_calib" << i << ".jpg";
        ifstream file(name.str());
        if(file.good()){
            Mat temp = imread(name.str());
            cam[0].savedImages.push_back(temp);
            file.close();
        }
    }

    name.str("");
    name << "CameraCalibration0" << u;

    ostringstream camtitle[3];
    camtitle[0].str("");
    camtitle[0] << "K" << u;
    camtitle[1].str("");
    camtitle[1] << "D" << u;
    camtitle[2].str("");
    camtitle[2] << "RMS" << u;

    Mat cameraMatrix, distCoefficients;

    double rms = cameraCalibration(cam[0].savedImages, cameraMatrix, distCoefficients);

    FileStorage fs1("../InternalParam.xml", FileStorage::APPEND);
    if( fs1.isOpened() ) {
        fs1 << camtitle[2].str() << rms;
        fs1 << camtitle[0].str() << cameraMatrix;
        fs1 << camtitle[1].str() << distCoefficients;
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_INTRINSIC_PARAMETERS\n";

    return 0;
}

//***********************PRIVATE**************************
void EyeBase::initializeCamera(unsigned i[], unsigned size){
    for (unsigned j = 0; j<size; j++){
        vid[i[j]] = new VideoCapture(cam[i[j]].vid);
        if(!vid[i[j]]->isOpened())
            std::cout << "ERROR: Failed to open camera" << std::endl;
        formatFeed(i[j]);
    }
}

Mat EyeBase::getFrame(unsigned camNum, bool warp){
    Mat warpedImage, croppedImage;
    vid[camNum]->read(cam[camNum].frame);
    if(warp==true){
        warpAffine(cam[camNum].frame, warpedImage, cam[camNum].rotate, cam[camNum].bbox.size());  
        cam[camNum].frame = warpedImage(cam[camNum].ROI);
    }
    return cam[camNum].frame;
}

void EyeBase::formatFeed(unsigned camNum){
    Mat src;
    // Prepare data for image rotation
    vid[camNum]->read(src);

    Point2f center(src.cols/2.0, src.rows/2.0);
    cam[camNum].rotate = cv::getRotationMatrix2D(center, cam[camNum].angle, 1.0);
    // determine bounding rectangle
    cam[camNum].bbox = cv::RotatedRect(center,src.size(), cam[camNum].angle).boundingRect();
    // adjust transformation matrix
    cam[camNum].rotate.at<double>(0,2) += cam[camNum].bbox.width/2.0 - center.x;
    cam[camNum].rotate.at<double>(1,2) += cam[camNum].bbox.height/2.0 - center.y;
}

/*
 * Core intrinsic calibration method.
 */
double EyeBase::cameraCalibration(vector<Mat> calibrationImages, Mat& cameraMatrix, Mat& distCoefficients){
    vector<vector<Point2f>> checkerboardImageSpacePoints;

    getChessboardCorners(calibrationImages, checkerboardImageSpacePoints, false);

    vector<vector<Point3f>> worldSpaceCornerPoints(1);

    createKnownBoardPosition(worldSpaceCornerPoints[0]);
    worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(),worldSpaceCornerPoints[0]);

    vector<Mat> rVectors, tVectors;
    distCoefficients = Mat::zeros(8,1, CV_64F);

    double rms = calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints, boardSize, cameraMatrix, distCoefficients, rVectors, tVectors,
        CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST +
                    CV_CALIB_SAME_FOCAL_LENGTH + CV_CALIB_RATIONAL_MODEL +
                    CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);

    return rms;
}

void EyeBase::createKnownBoardPosition(vector<Point3f>& corners){
    for(int i=0; i<boardSize.height; i++){
        for(int j=0; j<boardSize.width; j++){
            corners.push_back(Point3f(j*squareEdgeLength, i*squareEdgeLength, 0.0f));
        }
    }
}

bool EyeBase::getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false){
    bool found;
    vector<Point2f> pointBuf;

    for(vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++){
        found = findChessboardCorners(*iter, Size(9,6), pointBuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

        if(found){
            allFoundCorners.push_back(pointBuf);
        }

        if(showResults){
            drawChessboardCorners(*iter, Size(9,6), pointBuf, found);
            imshow("Looking for Corners", *iter);
            waitKey(0);
        }
    }
    return found;
}

/*
 * Core folder manipulation
 */
unsigned EyeBase::checkFolder(unsigned u, unsigned start, unsigned end){
    unsigned totalImg = 0;
    ostringstream name;
    ostringstream nameOverlay;
    
    for(unsigned i=start; i<end; i++){
        name.str("");
        name << "../../CalibrationPictures/Cam0" << cam[u].num << "/CAM0" << cam[u].num << "_calib" << i << ".jpg";
        ifstream file(name.str());
        if(file.good()){
            totalImg++;
            file.close();
        }
    }

    return totalImg;
}

bool EyeBase::rmFolder(string u){
    ostringstream name;
    name.str();
    name << "exec rm -r ../../CalibrationPictures/" << u;
    system(name.str().c_str());
    return true;
}

// This function creates the 3D points for the chessboard in its own coordinate system
vector<Point3f> EyeBase::Create3DChessboardCorners(Size boardSize, float squareSize){
    vector<Point3f> corners;

    for( int i = 0; i < boardSize.height; i++ ){
        for( int j = 0; j < boardSize.width; j++ ){
          corners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
        }
    }
    return corners;
}



