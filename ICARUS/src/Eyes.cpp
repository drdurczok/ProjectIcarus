#include "Eyes.h"

Eyes::Eyes()
{

}

Eyes::~Eyes()
{
    //dtor
}

bool Eyes::loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficient){
    ifstream inStream(name);
    if(inStream){
        uint16_t rows;
        uint16_t columns;

        inStream >> rows;
        inStream >> columns;

        cameraMatrix = Mat(Size(columns, rows), CV_64F);

        for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double read = 0.0f;
                inStream >> read;
                cameraMatrix.at<double>(r,c) = read;
                cout << cameraMatrix.at<double>(r,c) << "\n";
            }
        }
        //Distance Coefficients
        inStream >> rows;
        inStream >> columns;

        distanceCoefficient = Mat::zeros(rows, columns, CV_64F);

        for(int r=0; r<rows; r++){
            for(int c=0; c<columns; c++){
                double read = 0.0f;
                inStream >> read;
                distanceCoefficient.at<double>(r,c) = read;
                cout << distanceCoefficient.at<double>(r,c) << "\n";
            }
        }
        inStream.close();
        return true;
    }
    return false;
}

void Eyes::depthMap(){
    cv::VideoCapture Rcam(1);
    cv::VideoCapture Lcam(2);

    cv::Mat LeftImgOrg;
    cv::Mat RightImgOrg;
    cv::Mat LeftImgGrey;
    cv::Mat RightImgGrey;

    cv::Mat disp, disp8;

    char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && Rcam.isOpened() && Lcam.isOpened()) {		// until the Esc key is pressed or webcam connection is lost
		if (!Rcam.read(RightImgOrg) || !Lcam.read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
			std::cout << "error: frame not read from webcam\n";
			break;
		}

        cv::cvtColor(LeftImgOrg, LeftImgGrey, CV_BGR2GRAY);
        cv::cvtColor(RightImgOrg, RightImgGrey, CV_BGR2GRAY);

        cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create(16,21);
        sbm->setDisp12MaxDiff(1);
        sbm->setSpeckleRange(8);
        sbm->setSpeckleWindowSize(9);
        sbm->setUniquenessRatio(0);
        sbm->setTextureThreshold(507);
        sbm->setMinDisparity(-39);
        sbm->setPreFilterCap(61);
        sbm->setPreFilterSize(5);
        sbm->compute(LeftImgGrey,RightImgGrey,disp);
        normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

        imshow("left", LeftImgOrg);
        imshow("right", RightImgOrg);
        imshow("disp", disp8);

        charCheckForEscKey = cv::waitKey(1);			// delay (in ms) and get key press, if any
    }
}

void Eyes::depthMapIMG(){
    cv::Mat img1, img2, g1, g2;
    cv::Mat disp, disp8;

    img1 = cv::imread("leftImage.jpg");
    img2 = cv::imread("rightImage.jpg");

    cvtColor(img1, g1, CV_BGR2GRAY);
    cvtColor(img2, g2, CV_BGR2GRAY);

    cv::Ptr<cv::StereoBM> sbm = cv::StereoBM::create(16,21);
    sbm->setDisp12MaxDiff(1);
    sbm->setSpeckleRange(8);
    sbm->setSpeckleWindowSize(9);
    sbm->setUniquenessRatio(0);
    sbm->setTextureThreshold(507);
    sbm->setMinDisparity(-39);
    sbm->setPreFilterCap(61);
    sbm->setPreFilterSize(5);
    sbm->compute(g1,g2,disp);
    normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);

    cv::imshow("left", img1);
    cv::imshow("right", img2);
    cv::imshow("disp", disp8);

    char charCheckForEscKey = 0;
	while (charCheckForEscKey != 27) { charCheckForEscKey = cv::waitKey(1);}
}

unsigned Eyes::recordVideo(){
    cv::VideoCapture cam(1);
    if (cam.isOpened() == false) {				// check if VideoCapture object was associated to webcam successfully
		std::cout << "error: capWebcam not accessed successfully\n\n";

		return 0;
	}

    cv::Mat imgOriginal;
    cv::Mat imgGrey;
    cv::Mat imgThresh;

    cv::namedWindow("imgOriginal",CV_WINDOW_AUTOSIZE);
    cv::namedWindow("imgThresh",CV_WINDOW_AUTOSIZE);

    int count = 0;
    char charCheckForEscKey = 0;

	while (charCheckForEscKey != 27 && cam.isOpened()) {		// until the Esc key is pressed or webcam connection is lost
        bool blnFrameReadSuccessfully = cam.read(imgOriginal);		// get next frame
		if (!blnFrameReadSuccessfully || imgOriginal.empty()) {
			std::cout << "error: frame not read from webcam\n";
			break;
		}
        cv::cvtColor(imgOriginal,imgGrey,CV_RGB2GRAY);

        cv::GaussianBlur(imgGrey,imgGrey,cv::Size(15,15),0.0,0);    // noise reduction
        cv::threshold(imgGrey,imgThresh,0,255,cv::THRESH_BINARY_INV+cv::THRESH_OTSU);

        std::vector<std::vector<cv::Point> >contours;
        std::vector<cv::Vec4i>hierarchy;
        cv::findContours(imgThresh,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cv::Point());
        if(contours.size()>0){
            size_t indexOfBiggestContour = -1;
            size_t sizeOfBiggestContour = 0;

            for (size_t i = 0; i < contours.size(); i++){
                if(contours[i].size() > sizeOfBiggestContour){
                    sizeOfBiggestContour = contours[i].size();
                    indexOfBiggestContour = i; }
            }

            std::vector<std::vector<int> >hull(contours.size());
            std::vector<std::vector<cv::Point> >hullPoint(contours.size());
            std::vector<std::vector<cv::Vec4i> >defects(contours.size());
            std::vector<std::vector<cv::Point> >defectPoint(contours.size());
            std::vector<std::vector<cv::Point> >contours_poly(contours.size());

            for(size_t i=0;i<contours.size();i++){
                if(cv::contourArea(contours[i])>5000 && cv::contourArea(contours[i])<70000){
                    cv::convexHull(contours[i],hull[i],true);
                    cv::convexityDefects(contours[i],hull[i],defects[i]);
                    if(indexOfBiggestContour==i){
                        for(size_t k=0;k<hull[i].size();k++){
                            int ind=hull[i][k];
                            hullPoint[i].push_back(contours[i][ind]);
                        }
                        count =0;

                        for(size_t k=0;k<defects[i].size();k++){
                            if(defects[i][k][3]>13*256){
                                /*   int p_start=defects[i][k][0];   */
                                int p_end=defects[i][k][1];
                                int p_far=defects[i][k][2];
                                defectPoint[i].push_back(contours[i][p_far]);
                                cv::circle(imgThresh,contours[i][p_end],3,cv::Scalar(0,255,0),2);
                                count++; }
                        }
                    std::cout << count << std::endl;
                    drawContours(imgThresh, contours, i,cv::Scalar(255,255,0),2, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
                    drawContours(imgThresh, hullPoint, i, cv::Scalar(255,255,0),1, 8, std::vector<cv::Vec4i>(),0, cv::Point());
                    drawContours(imgOriginal, hullPoint, i, cv::Scalar(0,0,255),2, 8, std::vector<cv::Vec4i>(),0, cv::Point() );
                    }
                }
            }

        }

        cv::imshow("imgOriginal", imgOriginal);			// show windows
        cv::imshow("imgThresh", imgThresh);

        charCheckForEscKey = cv::waitKey(1);			// delay (in ms) and get key press, if any
    }


    return 0;
}
