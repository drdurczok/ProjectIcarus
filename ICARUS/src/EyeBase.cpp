#include "../include/EyeBase.h"

EyeBase::EyeBase()
{
    squareEdgeLength = 0.025f; //meters
    board_height = 6;
    board_width = 9;
    boardSize = Size(board_height,board_width);

    cam[1].num = 1;
    cam[1].title = "LCam";
    cam[1].posx = 20;
    cam[1].angle = 90.;
    //Maximum size is Rect(0, 0, 481, 641)
    unsigned offsetx = 30;
    unsigned offsety = 20;
    cam[1].ROI = Rect(1, 10+offsety, 480-offsetx, 600);

    cam[2].num = 2;
    cam[2].title = "RCam";
    cam[2].posx = 700;
    cam[2].angle = -90.;
    cam[2].ROI = Rect(offsetx, 10, 479-offsetx, 600); 
}

EyeBase::~EyeBase()
{
    vid[1]->release();
    vid[2]->release();
}

/*
 * This method produces the intrinsic camera parameters from a set of previously saved images.
 * The parameters are stored in an XML file for future reference along with a RMS(root mean square)
 * value that depicts the pixel deviation. A value anywhere from 0.1 to 1.0 is considered a good
 * calibration.
 */

unsigned EyeBase::calibrationFromFiles(unsigned u, unsigned start, unsigned end){
    cam[0].savedImages.clear();
    for(unsigned i=start; i<end; i++){
        cam[0].name.str("");
        cam[0].name << "../../CalibrationPictures/Cam0" << u << "/CAM0" << u << "_calib" << i << ".jpg";
        ifstream file(cam[0].name.str());
        if(file.good()){
            Mat temp = imread(cam[0].name.str());
            cam[0].savedImages.push_back(temp);
            file.close();
        }
    }

    cam[0].name.str("");
    cam[0].name << "CameraCalibration0" << u;

    ostringstream camtitle[3];
    camtitle[0].str("");
    camtitle[0] << "K" << u;
    camtitle[1].str("");
    camtitle[1] << "D" << u;
    camtitle[2].str("");
    camtitle[2] << "RMS" << u;

    double rms = cameraCalibration(cam[0].savedImages, cam[0].cameraMatrix, cam[0].distCoefficients);

    FileStorage fs1("../InternalParam.xml", FileStorage::APPEND);
    if( fs1.isOpened() ) {
        fs1 << camtitle[2].str() << rms;
        fs1 << camtitle[0].str() << cam[0].cameraMatrix;
        fs1 << camtitle[1].str() << cam[0].distCoefficients;
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_INTRINSIC_PARAMETERS\n";

    return 0;
}

void EyeBase::swapCameras(){
    vid[1]->release();
    vid[2]->release();

    unsigned temp = camVid[0];
    camVid[0] = camVid[1];
    camVid[1] = temp;

    unsigned i[2] = {1, 2};
    initializeCamera(i,2);
}

//***********************PRIVATE**************************
void EyeBase::initializeCamera(unsigned i[], unsigned size){
    for (unsigned j = 0; j<size; j++){
        vid[i[j]] = new VideoCapture(camVid[j]);
        if(!vid[i[j]]->isOpened())
            std::cout << "ERROR: Failed to open camera" << std::endl;
        formatData(i[j]);
    }
}

Mat EyeBase::getFrame(unsigned camNum){
    Mat warpedImage, croppedImage;
    vid[camNum]->read(cam[camNum].frame);
    warpAffine(cam[camNum].frame, warpedImage, cam[camNum].rotate, cam[camNum].bbox.size());  
    croppedImage = warpedImage(cam[camNum].ROI);

    return croppedImage;
}

void EyeBase::formatData(unsigned camNum){
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

    double rms = calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints, boardSize, cameraMatrix, distCoefficients, rVectors, tVectors);

    return rms;
}

void EyeBase::createKnownBoardPosition(vector<Point3f>& corners){
    for(int i=0; i<boardSize.height; i++){
        for(int j=0; j<boardSize.width; j++){
            corners.push_back(Point3f(j*squareEdgeLength, i*squareEdgeLength, 0.0f));
        }
    }
}

void EyeBase::getChessboardCorners(vector<Mat> images, vector<vector<Point2f>>& allFoundCorners, bool showResults = false){
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

/*
 * Core folder manipulation
 */
unsigned EyeBase::checkFolder(unsigned u, unsigned start = 0, unsigned end = 399){
    unsigned totalImg = 0;

    for(unsigned i=start; i<end; i++){
        cam[0].name.str("");
        cam[0].name << "../../CalibrationPictures/Cam0" << cam[u].num << "/CAM0" << cam[u].num << "_calib" << i << ".jpg";
        ifstream file(cam[0].name.str());
        if(file.good()){
            totalImg++;
            file.close();
        }
    }

    return totalImg;
}

bool EyeBase::rmFolder(unsigned u){
    ostringstream name;
    name.str();
    name << "exec rm -r ../../CalibrationPictures/Cam0" << u << "/*";
    system(name.str().c_str());
    return true;
}

/*
 * Based on intrinsic and extrinsic calibration parameters, remapping matrices are produced to
 * create the rectified image. The RMap values are stored in an XML file for future reference.
 */
void EyeBase::createRMap(Mat& LeftImgOrg, Mat& RightImgOrg){
    Mat distCoefficients[2];
    Mat cameraMatrix[2], R[2], P[2];
    Mat rmap[2][2];

    FileStorage fs1("../CalibrationParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["R1"] >> R[0];
        fs1["R2"] >> R[1];
        fs1["P1"] >> P[0];
        fs1["P2"] >> P[1];
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to READ_EXTRINSIC_PARAMETERS\n";

    fs1.open("../InternalParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["K1"] >> cameraMatrix[0];
        fs1["K2"] >> cameraMatrix[1];
        fs1["D1"] >> distCoefficients[0];
        fs1["D2"] >> distCoefficients[1];
        fs1.release();
    }
    else
        cout << "Error: Couldn't open InternalParam.xml to READ_INTERNAL_PARAMETERS\n";

    initUndistortRectifyMap(cameraMatrix[0], distCoefficients[0], R[0], P[0], LeftImgOrg.size(), CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(cameraMatrix[1], distCoefficients[1], R[1], P[1], RightImgOrg.size(), CV_16SC2, rmap[1][0], rmap[1][1]);

    fs1.open("../RMapParam.xml", FileStorage::WRITE);
    if( fs1.isOpened() ) {
        fs1 << "RMap00" << rmap[0][0];
        fs1 << "RMap01" << rmap[0][1];
        fs1 << "RMap10" << rmap[1][0];
        fs1 << "RMap11" << rmap[1][1];
        fs1.release();
    }
    else
        cout << "Error: Couldn't open RMapParam.xml to WRITE_MAPPING_PARAMETERS\n";
}



