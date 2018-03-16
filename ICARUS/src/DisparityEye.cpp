#include "../include/DisparityEye.h"

DisparityEye::DisparityEye()
{
    //ctor
}

DisparityEye::~DisparityEye()
{
    //dtor
}

void DisparityEye::depthMap(){
    VideoCapture Rcam(1);
    VideoCapture Lcam(2);

    Mat LeftImgOrg;
    Mat RightImgOrg;
    Mat LeftImgGrey;
    Mat RightImgGrey;

    Mat disp, disp8;

    char charKey = 0;

    Mat distanceCoefficients01;
    Mat cameraMatrix01;
    Mat distanceCoefficients02;
    Mat cameraMatrix02;

    loadCameraCalibration("../CameraCalibration01", cameraMatrix01, distanceCoefficients01);
    loadCameraCalibration("../CameraCalibration02", cameraMatrix02, distanceCoefficients02);

	Mat undistorted01;
	Mat undistorted02;


	int minDisparity = 16;
	int numDisparities = 16;
	int SADWindowSize = 16;
    int P1 = 0;
    int P2 = 0;
    int disp12MaxDiff = 0;
    int preFilterCap = 10;
    int uniquenessRatio = 0;
    int speckleWindowSize = 0;
    int speckleRange = 0;
    bool fullDP = false;

    int state = 0;

	while (charKey != 27 && Rcam.isOpened() && Lcam.isOpened()) {		// until the Esc key is pressed or webcam connection is lost
		if (!Rcam.read(RightImgOrg) || !Lcam.read(LeftImgOrg) || RightImgOrg.empty() || LeftImgOrg.empty()) {
			std::cout << "error: frame not read from webcam\n";
			break;
		}

		charKey = waitKey(1);			// delay (in ms) and get key press, if any
		GUI(minDisparity, numDisparities, SADWindowSize, P1, P2, disp12MaxDiff, preFilterCap, uniquenessRatio, speckleWindowSize, speckleRange, fullDP, charKey, state);

    	undistort(LeftImgOrg, undistorted01, cameraMatrix01, distanceCoefficients01);
    	undistort(RightImgOrg, undistorted02, cameraMatrix02, distanceCoefficients02);

        cvtColor(undistorted01, LeftImgGrey, CV_BGR2GRAY);
        cvtColor(undistorted02, RightImgGrey, CV_BGR2GRAY);
        
        /*
        Ptr<StereoBM> sbm = StereoBM::create(16,21);
        sbm->setDisp12MaxDiff(a);			//Maximum allowed difference (in integer pixel units) in the left-right disparity check
        sbm->setSpeckleRange(b);			//Maximum disparity variation within each connected component
        sbm->setSpeckleWindowSize(c);		//Maximum size of smooth disparity regions to consider their noise speckles and invalidate
        sbm->setUniquenessRatio(d);			//Margin in percentage by which the best (minimum) computed cost function value should “win” the second best value to consider the found match correct. Normally, a value within the 5-15 range is good enough
        sbm->setTextureThreshold(e);
        sbm->setMinDisparity(f);			//Minimum possible disparity value
        sbm->setPreFilterCap(g);			//[1..63] Truncation value for the prefiltered image pixels
        sbm->setPreFilterSize(h);			//[5..255]
        sbm->compute(LeftImgGrey,RightImgGrey,disp);
        normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);
*/

        Ptr<StereoSGBM> sgbm = StereoSGBM::create(minDisparity, numDisparities, SADWindowSize,
												               P1, P2, disp12MaxDiff, preFilterCap,uniquenessRatio,
												               speckleWindowSize, speckleRange, fullDP);
		sgbm->compute(LeftImgGrey,RightImgGrey, disp);
        normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);


        imshow("left", LeftImgOrg);
        imshow("right", RightImgOrg);
        imshow("disp", disp8);

        //charKey = waitKey(1);			// delay (in ms) and get key press, if any
    }
}

void DisparityEye::GUI(int &a, int &b, int &c, int &d, int &e, int &f, int &g, int &h, int &i, int &j, bool &k, char charKey, int &state){
	if (charKey == 'Q' || charKey == 'S' || charKey == 'T' || charKey == 'R') { //R up, T down, S right, Q left
		printf("\e[2J\e[H");
		int steps = 20;
		
		switch(charKey){
			case 'R': state--; break;
			case 'T': state++; break;
			case 'S': {
				switch(state){
					case 0: a++; break;
					case 1: b=b+16; break;
					case 2: c++; break;
					case 3: d++; break;
					case 4: e++; break;
					case 5: f++; break;
					case 6: g++; break;
					case 7: h++; break;
					case 8: i++; break;
					case 9: j++; break;
					case 10: !k; break;
					default: break;
				}
			} break;
			case 'Q': {
				switch(state){
					case 0: a--; break;
					case 1: b=b-16; break;
					case 2: c--; break;
					case 3: d--; break;
					case 4: e--; break;
					case 5: f--; break;
					case 6: g--; break;
					case 7: h--; break;
					case 8: i--; break;
					case 9: j--; break;
					case 10: !k; break;
					default: break;
				}
			} break;
			default: break;
		}
		if(state < 0) state = 10;
		else if(state > 10) state = 0;

		if(state != 0)  std::cout << "MinDisparity       |"; 
		else 			std::cout << "\033[1;31mMinDisparity\033[0m       |"; 
		LoadBar(-100,100,a,steps);

		if(state != 1)  std::cout << "numDisparities     |"; 
		else 			std::cout << "\033[1;31mnumDisparities\033[0m     |"; 
		LoadBar(0,16*10,b,steps);

		if(state != 2)  std::cout << "SADWindowSize      |"; 
		else 			std::cout << "\033[1;31mSADWindowSize\033[0m      |";
		LoadBar(0,100,c,steps);

		if(state != 3)  std::cout << "P1                 |"; 
		else 			std::cout << "\033[1;31mP1\033[0m                 |";
		LoadBar(0,100,d,steps);

		if(state != 4)  std::cout << "P2                 |"; 
		else 			std::cout << "\033[1;31mP2\033[0m                 |";
		LoadBar(0,100,e,steps);

		if(state != 5)  std::cout << "disp12MaxDiff      |"; 
		else 			std::cout << "\033[1;31mdisp12MaxDiff\033[0m      |";
		LoadBar(0,100,f,steps);

		if(state != 6)  std::cout << "preFilterCap       |"; 
		else 			std::cout << "\033[1;31mpreFilterCap\033[0m       |";
		LoadBar(0,100,g,steps);

		if(state != 7)  std::cout << "uniquenessRatio    |"; 
		else 			std::cout << "\033[1;31muniquenessRatio\033[0m    |";
		LoadBar(0,100,h,steps);

		if(state != 8)  std::cout << "speckleWindowSize  |"; 
		else 			std::cout << "\033[1;31mspeckleWindowSize\033[0m  |";
		LoadBar(0,100,i,steps);

		if(state != 9)  std::cout << "speckleRange       |"; 
		else 			std::cout << "\033[1;31mspeckleRange\033[0m       |";
		LoadBar(0,100,j,steps);

		if(state != 10) std::cout << "fullDP             |" << std::endl;
		else 			std::cout << "\033[1;31mfullDP\033[0m             |" << std::endl; 
	}
}

void DisparityEye::LoadBar(int min, int max, int &val, int steps){
	int interval = (max-min)/steps;
	if (val > max) val = max;
	else if (val < min) val = min;
	int progress = (val-min)/interval;

	for (unsigned i = 0; i<progress; i++){
		std::cout << "|";
	}	

	std::cout << "   " << val << std::endl;
}