#include "../include/DisparityEye.h"

DisparityEye::DisparityEye()
{
    unsigned i[2] = {1, 2};
    initializeCamera(i,2);
}

DisparityEye::~DisparityEye()
{

}

void DisparityEye::showDepthMap(){
    Mat LeftImgOrg(cam[cam[1].vid].size.width, cam[cam[1].vid].size.height, CV_8UC3, Scalar(0,0,255));
    Mat RightImgOrg(cam[cam[2].vid].size.width, cam[cam[2].vid].size.height, CV_8UC3, Scalar(0,0,255));
    Mat LeftImgGrey;
    Mat RightImgGrey;
    Mat undistorted[2];
    Mat disp, disp8;

    char charKey = 0;
    //int state = 0;

    namedWindow("left", CV_WINDOW_AUTOSIZE);
    moveWindow("left", 20, 20);
    namedWindow("right", CV_WINDOW_AUTOSIZE);
    moveWindow("right", 1380, 20);
    namedWindow("disp", CV_WINDOW_AUTOSIZE);
    moveWindow("disp", 700, 20);

    Mat rmap[2][2];

    FileStorage fs1("../RMapParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["RMap00"] >> rmap[0][0];
        fs1["RMap01"] >> rmap[0][1];
        fs1["RMap10"] >> rmap[1][0];
        fs1["RMap11"] >> rmap[1][1];
        fs1.release();
    }
    else {
        cout << "Error: Couldn't open CalibrationParam.xml to READ_RMAP_PARAMETERS\n";
    }

	int minDisparity = 0;			//Minimum possible disparity value. Normally, it is zero but sometimes rectification algorithms can shift images, so this parameter needs to be adjusted accordingly.
	int numDisparities = 1;			//Maximum disparity minus minimum disparity. The value is always greater than zero. In the current implementation, this parameter must be divisible by 16.
	int SADWindowSize = 5;			//Set size of matching blocks (1 for a single pixel)
    int P1 = 0;						//The first parameter controlling the disparity smoothness.  P1 is the penalty on the disparity change by plus or minus 1 between neighbor pixels.
    int P2 = 1;					    //The second parameter controlling the disparity smoothness. The larger the values are, the smoother the disparity is.  P2 is the penalty on the disparity change by more than 1 between neighbor pixels. The algorithm requires P2 > P1 
    int disp12MaxDiff = 0;			//Maximum allowed difference (in integer pixel units) in the left-right disparity check. Set it to a non-positive value to disable the check.
    int preFilterCap = 10;			//Truncation value for the prefiltered image pixels.
    int uniquenessRatio = 0;		//Margin in percentage by which the best (minimum) computed cost function value should “win” the second best value to consider the found match correct. Normally, a value within the 5-15 range is good enough.
    int speckleWindowSize = 0;		//Maximum size of smooth disparity regions to consider their noise speckles and invalidate. Set it to 0 to disable speckle filtering. Otherwise, set it somewhere in the 50-200 range.
    int speckleRange = 0;			//Maximum disparity variation within each connected component. If you do speckle filtering, set the parameter to a positive value, it will be implicitly multiplied by 16. Normally, 1 or 2 is good enough.
    bool fullDP = false;			//False - single pass, 5 directions, Trues - 8 directions
	
	string windowName = "disp";
	cv::createTrackbar("minDisparity", windowName, &minDisparity, 360);
	cv::createTrackbar("numDisparities", windowName, &numDisparities, 10);
	cv::createTrackbar("SADWindowSize", windowName, &SADWindowSize, 50);
	cv::createTrackbar("P1", windowName, &P1, 360);
	cv::createTrackbar("P2", windowName, &P2, 100);
	cv::createTrackbar("disp12MaxDiff", windowName, &disp12MaxDiff, 100);
	cv::createTrackbar("preFilterCap", windowName, &preFilterCap, 100);
	cv::createTrackbar("uniquenessRatio", windowName, &uniquenessRatio, 100);
	cv::createTrackbar("speckleWindowSize", windowName, &speckleWindowSize, 100);
	cv::createTrackbar("speckleRange", windowName, &speckleRange, 100);


//____________________________
	Mat distCoefficients[2];
    Mat cameraMatrix[2];

    fs1.open("../InternalParam.xml", FileStorage::READ);
    if( fs1.isOpened() ) {
        fs1["K1"] >> cameraMatrix[0];
        fs1["K2"] >> cameraMatrix[1];
        fs1["D1"] >> distCoefficients[0];
        fs1["D2"] >> distCoefficients[1];
        fs1.release();
    }
    else{
        cout << "Error: Couldn't open InternalParam.xml to READ_INTERNAL_PARAMETERS\n";
    }
//_____________________________

	while (charKey != 27 && vid[cam[2].vid]->isOpened() && vid[cam[1].vid]->isOpened()) {		// until the Esc key is pressed or webcam connection is lost
		if (!vid[cam[2].vid]->read(RightImgOrg) || !vid[cam[1].vid]->read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
			std::cout << "error: frame not read from webcam\n";
			break;
		}

        LeftImgOrg = getFrame(cam[1].vid);
        RightImgOrg = getFrame(cam[2].vid);


        /*
		charKey = waitKey(1);			// delay (in ms) and get key press, if any     
		GUI(minDisparity, numDisparities, SADWindowSize,
               P1, P2, disp12MaxDiff, preFilterCap,uniquenessRatio,
               speckleWindowSize, speckleRange, fullDP, charKey, state);   
		*/
        remap(LeftImgOrg, undistorted[0], rmap[0][0], rmap[0][1], INTER_LINEAR);
        remap(RightImgOrg , undistorted[1], rmap[1][0], rmap[1][1], INTER_LINEAR);

    	//undistort(LeftImgOrg, undistorted[0], cameraMatrix[0], distCoefficients[0]);
    	//undistort(RightImgOrg, undistorted[1], cameraMatrix[1], distCoefficients[1]);

    	//undistorted[0] = LeftImgOrg;
    	//undistorted[1] = RightImgOrg;

        Ptr<StereoSGBM> sgbm = StereoSGBM::create(minDisparity, numDisparities*16, SADWindowSize,
												               P1, P2, disp12MaxDiff, preFilterCap,uniquenessRatio,
												               speckleWindowSize, speckleRange, fullDP);
		sgbm->compute(undistorted[0],undistorted[1], disp);
        normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);


        imshow("left", undistorted[0]);
        imshow("right", undistorted[1]);
        imshow("disp", disp8);

        charKey = waitKey(1);			// delay (in ms) and get key press, if any
    }
    cvDestroyWindow("left");
    cvDestroyWindow("right");
    cvDestroyWindow("disp");
}

/*
 * Here we produce the extrinsic and rectification camera parameters for a stereo set.
 * This is accomplished with the stereoCalibrate() and stereoRectify() methods. 
 * Special attention is needed with their flags and parameter values.
 */
unsigned DisparityEye::stereoCalibration(unsigned num_imgs) {
    vector< vector< Point3f > > objectPoints;
    vector< vector< Point2f > > imgPoints[2];
    vector< Point2f > corners1, corners2;
    
    Mat img1, img2;
    Mat cameraMatrix[2], distCoefficients[2];

    ostringstream name[2];

    //Find Checkerboard Points
    vector< Point3f > obj;
    obj = Create3DChessboardCorners(boardSize, squareEdgeLength);

    bool found1 = false, found2 = false;
    for (unsigned i = 400; i <= num_imgs+399; i++) {
        name[0].str("");
        name[0] << "../../CalibrationPictures/Cam01/CAM01_calib" << i << ".jpg";
        name[1].str("");
        name[1] << "../../CalibrationPictures/Cam02/CAM02_calib" << i << ".jpg";
        img1 = imread(name[0].str(), CV_LOAD_IMAGE_COLOR);
        img2 = imread(name[1].str(), CV_LOAD_IMAGE_COLOR);

        found1 = false;
        found2 = false;

        found1 = findChessboardCorners(img1, boardSize, corners1, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        found2 = findChessboardCorners(img2, boardSize, corners2, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        
        if (found1 && found2) {
              cout << i << ". Found corners!" << endl;
              imgPoints[0].push_back(corners1);
              imgPoints[1].push_back(corners2);
              objectPoints.push_back(obj);
        }
    }

    
    Mat R, T, E, F;
    TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5);
    double rms = stereoCalibrate(objectPoints, imgPoints[0], imgPoints[1], 
                    cameraMatrix[0], distCoefficients[0], 
                    cameraMatrix[1], distCoefficients[1], 
                    img1.size(), R, T, E, F,
                    CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_RATIONAL_MODEL + CV_CALIB_FIX_PRINCIPAL_POINT +
                    CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);
    // + CV_CALIB_FIX_INTRINSIC
    
    FileStorage fs1("../CalibrationParam.xml", FileStorage::WRITE);
    if( fs1.isOpened() ) {
        fs1 << "ST_RMS" << rms;
        fs1 << "R" << R;
        fs1 << "T" << T;
        fs1 << "E" << E;
        fs1 << "F" << F;
        fs1 << "K1" << cameraMatrix[0];
        fs1 << "D1" << distCoefficients[0];
        fs1 << "K2" << cameraMatrix[1];
        fs1 << "D2" << distCoefficients[1];
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_EXTRINSIC_PARAMETERS\n";
  
    std::cout << "Done Calibration" << std::endl;

    std::cout << "Starting Rectification" << std::endl;

    Mat R1, R2, P1, P2, Q;
    stereoRectify(cameraMatrix[0], distCoefficients[0], 
                  cameraMatrix[1], distCoefficients[1], 
                  img1.size(), R, T, R1, R2, P1, P2, Q,
                  CALIB_ZERO_DISPARITY, 0);
    
    if( fs1.isOpened() ) {
        fs1 << "R1" << R1;
        fs1 << "R2" << R2;
        fs1 << "P1" << P1;
        fs1 << "P2" << P2;
        fs1 << "Q" << Q;
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_RECTIFICATION_PARAMETERS\n";

    createRMap();

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
        int npt = (int)imgPoints[0][i].size();
        Mat imgpt[2];
        for(unsigned k = 0; k < 2; k++ )
        {
            imgpt[k] = Mat(imgPoints[k][i]);
            undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoefficients[k], Mat(), cameraMatrix[k]);
            computeCorrespondEpilines(imgpt[k], k+1, F, lines[k]);
        }
        for(int j = 0; j < npt; j++ )
        {
            double errij = fabs(imgPoints[0][i][j].x*lines[1][j][0] +
                                imgPoints[0][i][j].y*lines[1][j][1] + lines[1][j][2]) +
                           fabs(imgPoints[1][i][j].x*lines[0][j][0] +
                                imgPoints[1][i][j].y*lines[0][j][1] + lines[0][j][2]);
            err += errij;
        }
        npoints += npt;
    }
    cout << "average epipolar err = " << err/npoints << endl;
    if( fs1.isOpened() ) {
        fs1 << "Epipolar_err" << err/npoints;
        fs1.release();
    }
    else
        cout << "Error: Couldn't open CalibrationParam.xml to APPEND_EPIPOLAR_ERROR\n";

    return 0;
}

//_______________________________VIDEO FEEDS___________________________________
void DisparityEye::cameraFeed(unsigned char flag = 0){
	printf("\033[2J\033[1;H\033[?25l"); 
	std::cout << "These are the camera feeds\n\n"
	        << "Press esc to continue" << std::endl;

	namedWindow("left", CV_WINDOW_AUTOSIZE);
    moveWindow("left", 20, 20);
    namedWindow("right", CV_WINDOW_AUTOSIZE);
    moveWindow("right", 710, 20);

    Mat LeftImgOrg, RightImgOrg;
    Mat LeftImg, RightImg;
    Mat cameraMatrix[2], distCoefficients[2];
    Mat rmap[2][2];

    if(flag & UNDISTORT){
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
	}
    if(flag & RECTIFY){
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
    }

    char charKey = 0;
    while (charKey != 27 && vid[2]->isOpened() && vid[1]->isOpened()) {       // until the Esc key is pressed or webcam connection is lost
        if (!vid[2]->read(RightImgOrg) || !vid[1]->read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
            std::cout << "error: frame not read from webcam\n";
            return;
        }

        charKey = waitKey(1);           // delay (in ms) and get key press, if any     

        LeftImgOrg = getFrame(cam[1].vid);
        RightImgOrg = getFrame(cam[2].vid);

        if(flag & UNDISTORT){
			undistort(LeftImgOrg, LeftImg, cameraMatrix[0], distCoefficients[0]);
        	undistort(RightImgOrg, RightImg, cameraMatrix[1], distCoefficients[1]);
        }
        else if(flag & RECTIFY){
			remap(LeftImgOrg, LeftImg, rmap[0][0], rmap[0][1], INTER_LINEAR);
	        remap(RightImgOrg, RightImg, rmap[1][0], rmap[1][1], INTER_LINEAR);
        }
        else{
        	LeftImg = LeftImgOrg;
        	RightImg = RightImgOrg;
        }

        imshow("left", LeftImg);
        imshow("right", RightImg);
    }

    cvDestroyWindow("left");
    cvDestroyWindow("right");
}

//___________________________________GUI______________________________________
void DisparityEye::GUI(){
	ostringstream header;
	header.str("");
	header << "This is the calibration menu\n\n";

	unsigned choice;
	
	string option[7];
	option[0] = "Disparity Map";
	option[1] = "Take Pictures";
	option[2] = "Create Calibration Parameters";
	option[3] = "Display Video Feeds";
	option[4] = "Display Undistorted Feeds";
	option[5] = "Display Rectified Feeds";
	option[6] = "Exit";

	bool loop = true;
	while(loop == true){
		choice = Menu(header.str(), option,  sizeof option/sizeof option[0]);

		switch(choice){
		  case 0: showDepthMap(); break;
		  case 1:  break;
		  case 2:  break;
		  case 3:  cameraFeed(); break;
		  case 4:  cameraFeed(UNDISTORT); break;
		  case 5:  cameraFeed(RECTIFY);   break;
		  case 6: loop = false;  break;
		  default: loop = false;
		}
	}
}


