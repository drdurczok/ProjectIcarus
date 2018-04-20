#include "../include/DisparityEye.h"

DisparityEye::DisparityEye()
{
	camVid[0] = 1;
    camVid[1] = 2;

    unsigned i[2] = {1, 2};
    initializeCamera(i,2);
}

DisparityEye::~DisparityEye()
{

}

void DisparityEye::showDepthMap(){
    Mat LeftImgOrg(cam[camVid[0]].size.width, cam[camVid[0]].size.height, CV_8UC3, Scalar(0,0,255));
    Mat RightImgOrg(cam[camVid[1]].size.width, cam[camVid[1]].size.height, CV_8UC3, Scalar(0,0,255));
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
	int numDisparities = 1;		//Maximum disparity minus minimum disparity. The value is always greater than zero. In the current implementation, this parameter must be divisible by 16.
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
	cv::createTrackbar("SADWindowSize", windowName, &SADWindowSize, 360);
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

	while (charKey != 27 && vid[camVid[1]]->isOpened() && vid[camVid[0]]->isOpened()) {		// until the Esc key is pressed or webcam connection is lost
		if (!vid[camVid[1]]->read(RightImgOrg) || !vid[camVid[0]]->read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
			std::cout << "error: frame not read from webcam\n";
			break;
		}

        LeftImgOrg = getFrame(camVid[0]);
        RightImgOrg = getFrame(camVid[1]);


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