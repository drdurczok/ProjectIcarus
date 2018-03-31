#include "../include/CalibrationEye.h"

CalibrationEye::CalibrationEye()
{

}

unsigned CalibrationEye::dispCamera(unsigned camNum){
    camera cam;
    VideoCapture vid(camNum);
    namedWindow("Cam", CV_WINDOW_AUTOSIZE);
    if(!vid.isOpened()) return 0;

    while(true){
        if(!vid.read(cam.frame)) break;
        imshow("Cam",cam.frame);
        char character = waitKey(1000/20);
        if (character == 27) break;
    }
    cvDestroyWindow("Cam");
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

unsigned CalibrationEye::calibration(unsigned cam_num = 0, bool saveData = true, bool dual = false, unsigned picIter = 1){
    camera cam, cam2;

    VideoCapture vid(cam_num);
    VideoCapture vid2(cam_num+1);
    if(!vid.isOpened()) return 0;

    namedWindow("Cam", CV_WINDOW_AUTOSIZE);
    if (dual == 1) namedWindow("Cam2", CV_WINDOW_AUTOSIZE);

    int framesPerSecond = 20;

    cam.i = picIter;
    cam2.i = picIter;

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
                        cam.name << "../../CalibrationPictures/Cam0" << cam_num << "/CAM0" << cam_num << "_calib" << cam.i << ".jpg";
                        cam.nameOverlay << "../../CalibrationPictures/CalibrationPicturesOverlay/CAM0" << cam_num << "_calib" << cam.i << "_overlay.jpg";
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
                            cam2.name << "../../CalibrationPictures/Cam02/CAM02_calib" << cam2.i << ".jpg";
                            cam2.nameOverlay << "../../CalibrationPictures/CalibrationPicturesOverlay/CAM02_calib" << cam2.i << "_overlay.jpg";
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
                    cam.name.str("");
                    cam.name << "../CameraCalibration0" << cam_num;
                    cameraCalibration(cam.savedImages, chessboardDimensions, calibrationSquareDimension, cam.cameraMatrix, cam.distanceCoefficients);
                    saveCameraCalibration(cam.name.str(), cam.cameraMatrix, cam.distanceCoefficients);

                     if (dual == 1) {
                        cameraCalibration(cam2.savedImages, chessboardDimensions, calibrationSquareDimension, cam2.cameraMatrix, cam2.distanceCoefficients);
                        saveCameraCalibration("../CameraCalibration02", cam2.cameraMatrix, cam2.distanceCoefficients);
                     }
                }
                break;
            case 27:       //escape
                //exit program
                return 0;
                break;
        }
    }
    return 0;
}

unsigned CalibrationEye::calibrationFromFiles(unsigned u){
    camera cam;

    for(int i=0; i<500; i++){
        cam.name.str("");
        cam.name << "../../CalibrationPictures/Cam0" << u << "/CAM0" << u << "_calib" << i << ".jpg";
        ifstream file(cam.name.str());
        if(file.good()){
            Mat temp = imread(cam.name.str());
            cam.savedImages.push_back(temp);
            file.close();
        }
    }

    cam.name.str("");
    cam.name << "CameraCalibration0" << u;

    cameraCalibration(cam.savedImages, chessboardDimensions, calibrationSquareDimension, cam.cameraMatrix, cam.distanceCoefficients);
    saveCameraCalibration(cam.name.str(), cam.cameraMatrix, cam.distanceCoefficients);

    return 0;
}

unsigned CalibrationEye::stereoCalibration(int num_imgs) {
    vector< vector< Point3f > > object_points;
    vector< Point2f > corners1, corners2;
    vector< vector< Point2f > > imgPoints[2], imgPoints_n[2];
    
    Mat img1, img2, gray1, gray2;

    ostringstream name[2];

    for (int i = 1; i <= num_imgs; i++) {
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







