#include "../include/Eyes.h"

/*
 * Initialization of the class involves setting the correct camera indexes.
 * If during setup the cameras are swapped, it is possible to switch the
 * indexes in software and share the correct setup with getCameraState().
 */
Eyes::Eyes()
{
    camVid[0] = 1;
    camVid[1] = 2;

    unsigned i[2] = {1, 2};
    initializeCamera(i,2);
}

Eyes::Eyes(unsigned Lcam, unsigned Rcam)
{
    camVid[0] = Lcam;
    camVid[1] = Rcam;

    unsigned i[2] = {1, 2};
    initializeCamera(i,2);
}

Eyes::~Eyes()
{
  
}


unsigned Eyes::getCameraState(unsigned u){
    return camVid[u];
}

void Eyes::swapCameras(){
    vid[1]->release();
    vid[2]->release();

    unsigned temp = camVid[0];
    camVid[0] = camVid[1];
    camVid[1] = temp;

    unsigned i[2] = {1, 2};
    initializeCamera(i,2);
}

/*
 * The following methods allow for a preview of the various camera states.
 * We may display the camera as is, undistorted based on intrinsic parameters,
 * or a rectified version that involves all the included calibration features.
 */
unsigned Eyes::dispCamera(int camNum){
    Mat src, dst;
    string title = cam[camVid[camNum]].title;

    namedWindow(title, CV_WINDOW_AUTOSIZE);
    if(!vid[camVid[camNum]]->isOpened()) return 0;

    while(true){
        dst = getFrame(camVid[camNum]);

        imshow(title,dst);
        char character = waitKey(1000/20);
        if (character == 27) break;
    }

    cvDestroyWindow(title.c_str());
    return 0;
}

unsigned Eyes::dualCameraFeed(){
    namedWindow("left", CV_WINDOW_AUTOSIZE);
    moveWindow("left", 20, 20);
    namedWindow("right", CV_WINDOW_AUTOSIZE);
    moveWindow("right", 710, 20);

    Mat LeftImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat RightImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));

    char charKey = 0;
    while (charKey != 27 && vid[2]->isOpened() && vid[1]->isOpened()) {       // until the Esc key is pressed or webcam connection is lost
        if (!vid[2]->read(RightImgOrg) || !vid[1]->read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
            std::cout << "error: frame not read from webcam\n";
            return 0;
        }

        charKey = waitKey(1);           // delay (in ms) and get key press, if any       

        RightImgOrg = getFrame(camVid[1]);
        LeftImgOrg = getFrame(camVid[0]);
        
        imshow("left", LeftImgOrg);
        imshow("right", RightImgOrg);
    }

    cvDestroyWindow("left");
    cvDestroyWindow("right");

    return 0;
}

unsigned Eyes::dispUndistImage(){
    namedWindow("left", CV_WINDOW_AUTOSIZE);
    moveWindow("left", 20, 20);
    namedWindow("right", CV_WINDOW_AUTOSIZE);
    moveWindow("right", 710, 20);

    Mat LeftImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat RightImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat undistorted[2];

    Mat distCoefficients[2];
    Mat cameraMatrix[2];

    FileStorage fs1("../InternalParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["K1"] >> cameraMatrix[0];
        fs1["K2"] >> cameraMatrix[1];
        fs1["D1"] >> distCoefficients[0];
        fs1["D2"] >> distCoefficients[1];
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to READ_INTRINSIC_PARAMETERS\n";

    char charKey = 0;
    while (charKey != 27 && vid[2]->isOpened() && vid[1]->isOpened()) {       // until the Esc key is pressed or webcam connection is lost
        if (!vid[2]->read(RightImgOrg) || !vid[1]->read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
            std::cout << "error: frame not read from webcam\n";
            return 0;
        }

        charKey = waitKey(1);           // delay (in ms) and get key press, if any       

        RightImgOrg = getFrame(camVid[1]);
        LeftImgOrg = getFrame(camVid[0]);

        undistort(LeftImgOrg, undistorted[0], cameraMatrix[0], distCoefficients[0]);
        undistort(RightImgOrg, undistorted[1], cameraMatrix[1], distCoefficients[1]);

        imshow("left", undistorted[0]);
        imshow("right", undistorted[1]);
    }

    cvDestroyWindow("left");
    cvDestroyWindow("right");
    return 0;
}

unsigned Eyes::dispRectImage(){
    namedWindow("left", CV_WINDOW_AUTOSIZE);
    moveWindow("left", 20, 20);
    namedWindow("right", CV_WINDOW_AUTOSIZE);
    moveWindow("right", 710, 20);

    Mat LeftImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat RightImgOrg(480, 640, CV_8UC3, Scalar(0,0,255));
    Mat undistorted[2];

    createRMap(LeftImgOrg,RightImgOrg);

    char charKey = 0;

    Mat rmap[2][2];

    FileStorage fs1("../RMapParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["RMap00"] >> rmap[0][0];
        fs1["RMap01"] >> rmap[0][1];
        fs1["RMap10"] >> rmap[1][0];
        fs1["RMap11"] >> rmap[1][1];
        fs1.release();
    }
    else
        cout << "Error: Couldn't open RMapParam.xml to READ_RMAP_PARAMETERS\n";

    while (charKey != 27 && vid[2]->isOpened() && vid[1]->isOpened()) {       // until the Esc key is pressed or webcam connection is lost
        if (!vid[2]->read(RightImgOrg) || !vid[1]->read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
            std::cout << "error: frame not read from webcam\n";
            return 0;
        }

        charKey = waitKey(1);           // delay (in ms) and get key press, if any 

        RightImgOrg = getFrame(camVid[1]);
        LeftImgOrg = getFrame(camVid[0]);       

        remap(LeftImgOrg, undistorted[0], rmap[0][0], rmap[0][1], INTER_LINEAR);
        remap(RightImgOrg , undistorted[1], rmap[1][0], rmap[1][1], INTER_LINEAR);

        imshow("left", undistorted[0]);
        imshow("right", undistorted[1]);
    }

    cvDestroyWindow("left");
    cvDestroyWindow("right");
    return 0;
}

/*
 * A general live method for locating a checkerboard pattern, taking and storing the images.
 */
unsigned Eyes::calibration(unsigned cam_start = 1, bool dual = false, bool saveData = true, unsigned picIter = 1){
    unsigned iter;
    if(dual==true) iter = cam_start+2;
    else iter = cam_start+1;

    for(unsigned i=cam_start; i<iter; i++){
        namedWindow(cam[i].title, CV_WINDOW_AUTOSIZE);
        moveWindow(cam[i].title, cam[i].posx, 20);
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

        for(unsigned i=cam_start; i<iter; i++){
            cam[i].frame = getFrame(i);
            found = findChessboardCorners(cam[i].frame, boardSize, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE); //Use CV_CALIB_FAST_CHECK to increase frames but lower accuracy
            cam[i].frame.copyTo(cam[i].drawToFrame);
            drawChessboardCorners(cam[i].drawToFrame, boardSize, foundPoints, found);

            if(found) imshow(cam[i].title, cam[i].drawToFrame);
            else imshow(cam[i].title, cam[i].frame);
        }
        
        char character = waitKey(1000/framesPerSecond);

        switch(character){
            case ' ': {
                //saving image
                for(unsigned i=cam_start; i<iter; i++){
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
            } break;
            case 27: {   //escape
                //exit program
                for(unsigned i=cam_start; i<iter; i++){
                    cvDestroyWindow(cam[i].title.c_str());
                }
                return 0;
            } break;
        }
    }
    return 0;
}

/*
 * Here we produce the extrinsic and rectification camera parameters for a stereo set.
 * This is accomplished with the stereoCalibrate() and stereoRectify() methods. 
 * Special attention is needed with their flags and parameter values.
 */
unsigned Eyes::stereoCalibration(unsigned num_imgs) {
    vector< vector< Point3f > > object_points;
    vector< Point2f > corners1, corners2;
    vector< vector< Point2f > > imgPoints[2], imgPoints_n[2];
    
    Mat img1, img2, gray1, gray2;

    ostringstream name[2];

    Mat undistorted[2];

    Mat distCoefficients[2];
    Mat cameraMatrix[2];

    FileStorage fs1("../InternalParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["K1"] >> cameraMatrix[0];
        fs1["K2"] >> cameraMatrix[1];
        fs1["D1"] >> distCoefficients[0];
        fs1["D2"] >> distCoefficients[1];
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to READ_INTRINSIC_PARAMETERS\n";
    

    for (unsigned i = 400; i <= num_imgs+399; i++) {
        name[0].str("");
        name[0] << "../../CalibrationPictures/Cam01/CAM01_calib" << i << ".jpg";
        name[1].str("");
        name[1] << "../../CalibrationPictures/Cam02/CAM02_calib" << i << ".jpg";
        img1 = imread(name[0].str(), CV_LOAD_IMAGE_COLOR);
        img2 = imread(name[1].str(), CV_LOAD_IMAGE_COLOR);

        undistort(img1, undistorted[0], cameraMatrix[0], distCoefficients[0]);
        undistort(img2, undistorted[1], cameraMatrix[1], distCoefficients[1]);

        cvtColor(undistorted[0], gray1, CV_BGR2GRAY);
        cvtColor(undistorted[1], gray2, CV_BGR2GRAY);

        bool found1 = false, found2 = false;

        found1 = cv::findChessboardCorners(undistorted[0], boardSize, corners1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        found2 = cv::findChessboardCorners(undistorted[1], boardSize, corners2, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if (found1) {
            cv::cornerSubPix(gray1, corners1, cv::Size(5, 5), cv::Size(-1, -1),
            cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(gray1, boardSize, corners1, found1);
        }
        if (found2) {
            cv::cornerSubPix(gray2, corners2, cv::Size(5, 5), cv::Size(-1, -1),
            cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            cv::drawChessboardCorners(gray2, boardSize, corners2, found2);
        }

        vector< Point3f > obj;
        for (unsigned i = 0; i < board_height; i++)
            for (unsigned j = 0; j < board_width; j++)
                obj.push_back(Point3f((float)j * squareEdgeLength, (float)i * squareEdgeLength, 0));

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

    Mat R, T, E, F;

    double rms = stereoCalibrate(object_points, imgPoints_n[0], imgPoints_n[1], 
                    cameraMatrix[0], distCoefficients[0], 
                    cameraMatrix[1], distCoefficients[1], 
                    undistorted[0].size(), R, T, E, F, CV_CALIB_FIX_INTRINSIC);

    fs1.open("../CalibrationParam.xml", FileStorage::WRITE);
    if( fs1.isOpened() ) {
        fs1 << "ST_RMS" << rms;
        fs1 << "R" << R;
        fs1 << "T" << T;
        fs1 << "E" << E;
        fs1 << "F" << F;
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_EXTRINSIC_PARAMETERS\n";
  
    std::cout << "Done Calibration" << std::endl;

    std::cout << "Starting Rectification" << std::endl;

    Mat R1, R2, P1, P2, Q;
    stereoRectify(cameraMatrix[0], distCoefficients[0], 
                  cameraMatrix[1], distCoefficients[1], 
                  undistorted[0].size(), R, T, R1, R2, P1, P2, Q,
                  CALIB_ZERO_DISPARITY, 0);
    
    if( fs1.isOpened() ) {
        fs1 << "R1" << R1;
        fs1 << "R2" << R2;
        fs1 << "P1" << P1;
        fs1 << "P2" << P2;
        fs1 << "Q" << Q;
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_RECTIFICATION_PARAMETERS\n";

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
            undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoefficients[k], Mat(), cameraMatrix[k]);
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



