#include "../include/CalibrationEye.h"

CalibrationEye::CalibrationEye()
{
    cam[1].num = 1;
    cam[1].vid = 1;
    cam[1].title = "LCam";
    cam[2].num = 2;
    cam[2].vid = 2;
    cam[2].title = "RCam";
}

unsigned CalibrationEye::dispCamera(unsigned camNum){
    cam[camNum].num = camNum;
    string title = cam[camNum].title;

    VideoCapture vid(cam[camNum].vid);
    namedWindow(title, CV_WINDOW_AUTOSIZE);
    if(!vid.isOpened()) return 0;

    while(true){
        if(!vid.read(cam[camNum].frame)) break;
        imshow(title,cam[camNum].frame);
        char character = waitKey(1000/20);
        if (character == 27) break;
    }

    cvDestroyWindow(title.c_str());
    vid.release();
    return 0;
}

unsigned CalibrationEye::dispRectImage(){
    VideoCapture Lcam(cam[1].vid);
    VideoCapture Rcam(cam[2].vid);
    namedWindow("left", CV_WINDOW_AUTOSIZE);
    namedWindow("right", CV_WINDOW_AUTOSIZE);

    Mat LeftImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat RightImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat undistorted[2];

    createRMap(LeftImgOrg,RightImgOrg);

    char charKey = 0;
    while (charKey != 27 && Rcam.isOpened() && Lcam.isOpened()) {       // until the Esc key is pressed or webcam connection is lost
        if (!Rcam.read(RightImgOrg) || !Lcam.read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
            std::cout << "error: frame not read from webcam\n";
            break;
        }

        charKey = waitKey(1);           // delay (in ms) and get key press, if any 

        Lcam.read(RightImgOrg);
        Rcam.read(LeftImgOrg);       

        remap(LeftImgOrg, undistorted[0], rmap[0][0], rmap[0][1], INTER_LINEAR);
        remap(RightImgOrg , undistorted[1], rmap[1][0], rmap[1][1], INTER_LINEAR);

        imshow("left", undistorted[0]);
        imshow("right", undistorted[1]);

        //charKey = waitKey(1);         // delay (in ms) and get key press, if any
    }
    cvDestroyWindow("left");
    cvDestroyWindow("right");
    Rcam.release();
    Lcam.release();
    return 0;
}

unsigned CalibrationEye::calibration(unsigned cam_start = 1, unsigned cam_end = 2, bool saveData = true, unsigned picIter = 1){
    VideoCapture *vid[cam_end+1-cam_start];

    for(unsigned i=cam_start; i<cam_end+1; i++){
        vid[i] = new VideoCapture(cam[i].vid);
        if(!vid[i]->isOpened()) {
            std::cout << "Failed to open camera " << i << std::endl;
            return 0;
        }
        namedWindow(cam[i].title, CV_WINDOW_AUTOSIZE);
        cam[i].i = picIter;
    }

    int framesPerSecond = 20;

    unsigned picNum = picIter;
    if(picNum > 399) picNum = picNum - 400;
    printf("\033[2J\033[1;H\033[?25l");
    std::cout  << "To take a picture use the space bar.\n" << "Press esc to finish taking pictures.\n\n"
               << "Number of calibration pictures: " << picNum << std::endl;

    while(true){
        vector<Vec2f> foundPoints;
        bool found = false;

        for(unsigned i=cam_start; i<cam_end+1; i++){
            vid[i]->read(cam[i].frame);
            found = findChessboardCorners(cam[i].frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE); //Use CV_CALIB_FAST_CHECK to increase frames but lower accuracy
            cam[i].frame.copyTo(cam[i].drawToFrame);
            drawChessboardCorners(cam[i].drawToFrame, chessboardDimensions, foundPoints, found);

            if(found) imshow(cam[i].title, cam[i].drawToFrame);
            else imshow(cam[i].title, cam[i].frame);
        }
        

        char character = waitKey(1000/framesPerSecond);

        switch(character){
            case ' ':
                //saving image
                for(unsigned i=cam_start; i<cam_end+1; i++){
                    if(found){
                        Mat temp;
                        cam[i].frame.copyTo(temp);
                        cam[i].savedImages.push_back(temp);
                        if(saveData == 1) {
                            cam[i].name.str("");
                            cam[i].nameOverlay.str("");
                            cam[i].name << "../../CalibrationPictures/Cam0" << cam[i].num << "/CAM0" << cam[i].num << "_calib" << cam[i].i << ".jpg";
                            cam[i].nameOverlay << "../../CalibrationPictures/CalibrationPicturesOverlay/CAM0" << cam[i].num << "_calib" << cam[i].i << "_overlay.jpg";
                            cam[i].i++;
                            imwrite(cam[i].name.str(),temp);
                            imwrite(cam[i].nameOverlay.str(),cam[i].drawToFrame);
                            if(i == cam_start){
                                printf("\033[2J\033[1;H\033[?25l");
                                picNum++;
                                std::cout  << "To take a picture use the space bar.\n" << "Press esc to finish taking pictures.\n\n"
                                           << "Number of calibration pictures: " << picNum << std::endl;
                            }
                        }
                    }
                }
                break;
            case 13:    //enter
                //start calibration
                for(unsigned i=cam_start; i<cam_end+1; i++){
                    if(cam[i].savedImages.size() > 15){
                        cam[i].name.str("");
                        cam[i].name << "../CameraCalibration0" << cam[i].num;
                        cameraCalibration(cam[i].savedImages, chessboardDimensions, calibrationSquareDimension, cam[i].cameraMatrix, cam[i].distanceCoefficients);
                        saveCameraCalibration(cam[i].name.str(), cam[i].cameraMatrix, cam[i].distanceCoefficients);

                }
            }
                break;
            case 27:       //escape
                //exit program
                for(unsigned i=cam_start; i<cam_end+1; i++){
                    cvDestroyWindow(cam[i].title.c_str());
                    vid[i]->release();
                }
                return 0;
                break;
        }
    }
    return 0;
}

unsigned CalibrationEye::calibrationFromFiles(unsigned u){
    for(int i=0; i<500; i++){
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

    cameraCalibration(cam[0].savedImages, chessboardDimensions, calibrationSquareDimension, cam[0].cameraMatrix, cam[0].distanceCoefficients);
    saveCameraCalibration(cam[0].name.str(), cam[0].cameraMatrix, cam[0].distanceCoefficients);

    return 0;
}

unsigned CalibrationEye::stereoCalibration(int num_imgs) {
    vector< vector< Point3f > > object_points;
    vector< Point2f > corners1, corners2;
    vector< vector< Point2f > > imgPoints[2], imgPoints_n[2];
    
    Mat img1, img2, gray1, gray2;

    ostringstream name[2];

    for (int i = 400; i <= num_imgs+399; i++) {
        name[0].str("");
        name[0] << "../../CalibrationPictures/Cam01/CAM01_calib" << i << ".jpg";
        name[1].str("");
        name[1] << "../../CalibrationPictures/Cam02/CAM02_calib" << i << ".jpg";
        img1 = imread(name[0].str(), CV_LOAD_IMAGE_COLOR);
        img2 = imread(name[1].str(), CV_LOAD_IMAGE_COLOR);
        cvtColor(img1, gray1, CV_BGR2GRAY);
        cvtColor(img2, gray2, CV_BGR2GRAY);

        bool found1 = false, found2 = false;

        found1 = cv::findChessboardCorners(img1, chessboardDimensions, corners1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        found2 = cv::findChessboardCorners(img2, chessboardDimensions, corners2, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if (found1) {
            cv::cornerSubPix(gray1, corners1, cv::Size(5, 5), cv::Size(-1, -1),
            cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(gray1, chessboardDimensions, corners1, found1);
        }
        if (found2) {
            cv::cornerSubPix(gray2, corners2, cv::Size(5, 5), cv::Size(-1, -1),
            cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(gray2, chessboardDimensions, corners2, found2);
        }

        vector< Point3f > obj;
        for (unsigned i = 0; i < board_height; i++)
            for (unsigned j = 0; j < board_width; j++)
                obj.push_back(Point3f((float)j * calibrationSquareDimension, (float)i * calibrationSquareDimension, 0));

        if (found1 && found2) {
              cout << i << ". Found corners!" << endl;
              imgPoints[0].push_back(corners1);
              imgPoints[1].push_back(corners2);
              object_points.push_back(obj);
        }
    }

    for (unsigned i = 0; i < imgPoints[0].size(); i++) {
    vector< Point2f > v1, v2;
        for (unsigned j = 0; j < imgPoints[0][i].size(); j++) {
            v1.push_back(Point2f((double)imgPoints[0][i][j].x, (double)imgPoints[0][i][j].y));
            v2.push_back(Point2f((double)imgPoints[1][i][j].x, (double)imgPoints[1][i][j].y));
        }
        imgPoints_n[0].push_back(v1);
        imgPoints_n[1].push_back(v2);
    }

    Mat distanceCoefficients[2];
    Mat cameraMatrix[2];

    loadCameraCalibration("../CameraCalibration01", cameraMatrix[0], distanceCoefficients[0]);
    loadCameraCalibration("../CameraCalibration02", cameraMatrix[1], distanceCoefficients[1]);

    Mat R, T, E, F;

    stereoCalibrate(object_points, imgPoints_n[0], imgPoints_n[1], cameraMatrix[0], distanceCoefficients[0], cameraMatrix[1], distanceCoefficients[1], img1.size(), R, T, E, F);

    FileStorage fs1("../CalibrationParam.xml", FileStorage::WRITE);
    fs1 << "K1" << cameraMatrix[0];
    fs1 << "K2" << cameraMatrix[1];
    fs1 << "D1" << distanceCoefficients[0];
    fs1 << "D2" << distanceCoefficients[1];
    fs1 << "R" << R;
    fs1 << "T" << T;
    fs1 << "E" << E;
    fs1 << "F" << F;
  
    std::cout << "Done Calibration" << std::endl;

    std::cout << "Starting Rectification" << std::endl;

    Mat R1, R2, P1, P2, Q;
    stereoRectify(cameraMatrix[0], distanceCoefficients[0], cameraMatrix[1], distanceCoefficients[1], img1.size(), R, T, R1, R2, P1, P2, Q);
    
    if( fs1.isOpened() ) {
        fs1 << "R1" << R1;
        fs1 << "R2" << R2;
        fs1 << "P1" << P1;
        fs1 << "P2" << P2;
        fs1 << "Q" << Q;
        fs1.release();
    }
    else
        cout << "Error: can not save the extrinsic parameters\n";

    std::cout << "Done Rectification" << std::endl;

    // CALIBRATION QUALITY CHECK
    // because the output fundamental matrix implicitly
    // includes all the output information,
    // we can check the quality of calibration using the
    // epipolar geometry constraint: m2^t*F*m1=0
    std::cout << "Checking Quality of Calibration" << std::endl;

    double err = 0;
    int npoints = 0;
    vector<Vec3f> lines[2];
    unsigned nimages = 2;
    for(unsigned i = 0; i < nimages; i++ )
    {
        int npt = (int)imgPoints_n[0][i].size();
        Mat imgpt[2];
        for(unsigned k = 0; k < 2; k++ )
        {
            imgpt[k] = Mat(imgPoints_n[k][i]);
            undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distanceCoefficients[k], Mat(), cameraMatrix[k]);
            computeCorrespondEpilines(imgpt[k], k+1, F, lines[k]);
        }
        for(int j = 0; j < npt; j++ )
        {
            double errij = fabs(imgPoints_n[0][i][j].x*lines[1][j][0] +
                                imgPoints_n[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
                           fabs(imgPoints_n[1][i][j].x*lines[0][j][0] +
                                imgPoints_n[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
            err += errij;
        }
        npoints += npt;
    }
    cout << "average epipolar err = " << err/npoints << endl;

    return 0;
}

void CalibrationEye::swapCameras(){
    cam[1].vid = 2;
    cam[2].vid = 1;
}

unsigned CalibrationEye::checkFolder(unsigned u, unsigned start = 0){
    unsigned totalImg = 0;

    for(int i=start; i<500; i++){
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

bool CalibrationEye::rmFolder(unsigned u){
    ostringstream name;
    name.str();
    name << "exec rm -r ../../CalibrationPictures/Cam0" << u << "/*";
    system(name.str().c_str());
    return true;
}

//***********************PRIVATE**************************
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