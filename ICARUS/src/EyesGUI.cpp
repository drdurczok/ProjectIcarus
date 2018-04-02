#include "../include/EyesGUI.h"

EyesGUI::EyesGUI(){
        camVid[0] = 1;
        camVid[1] = 2;
}

void EyesGUI::calibrationGUI(){
	Eyes* calib = new Eyes(camVid[0],camVid[1]);
	
	ostringstream header;
	header.str("");
	header << "This is the calibration menu\n\n";

	unsigned choice;
	
	string option[7];
	option[0] = "Check Camera Position";
	option[1] = "Calibrate Intrinsic Parameters";
	option[2] = "Calibrate Extrinsic and Rectification Parameters";
	option[3] = "Calibrate All";
	option[4] = "Undistort Cameras";
	option[5] = "Rectify Cameras";
	option[6] = "Exit";

	bool loop = true;
	while(loop == true){
		choice = Menu(header.str(), option,  sizeof option/sizeof option[0]);

		switch(choice){
		  case 0: swapCameras(calib); break;
		  case 1: intrinsicParam(calib); break;
		  case 2: extrinsicParam(calib); break;
		  case 3: calibration(calib); break;
		  case 4: break;
		  case 5: rectImage(calib); break;
		  case 6: loop = false; break;
		  default: loop = false;
		}
		camVid[0] = calib->getCameraState(1);
		camVid[1] = calib->getCameraState(2);
	}

	delete calib;
}

void EyesGUI::swapCameras(Eyes* calib){
	printf("\033[2J\033[1;H\033[?25l"); 
	std::cout << "Check whether this is the left camera then press esc." << std::endl;
	
	unsigned choice;
	ostringstream header;
    header.str("");
    header << "Confirm that this is the left camera.\n\n";
    string option[2];
    option[0] = "Correct camera selected";
    option[1] = "Swap cameras";

    calib->dispCamera(1);

    choice = Menu(header.str(), option, sizeof option/sizeof option[0]);

    printf("\033[2J\033[1;H\033[?25l"); 

    switch(choice){
      case 0: break;
      case 1: calib->swapCameras();
    }
}

void EyesGUI::intrinsicParam(Eyes* calib){
	ostringstream header;
	unsigned choice;

    for(unsigned i=1; i<3; i++){
      printf("\033[2J\033[1;H\033[?25l"); 
      header.str("");
      
      unsigned totalImg = calib->checkFolder(i,0,399);
      if(totalImg!=0){
        header << "We will begin with the intrinsic parameter calibration of individual cameras\n\n" 
               << "You already have " << totalImg << " calibration photos taken for camera " << i << "\n\n";
        string option2[4];
        option2[0] = "Use pictures";
        option2[1] = "Add more pictres";
        option2[2] = "Delete pictures and create new ones";
        option2[3] = "Skip this step";

        choice = Menu(header.str(), option2,  sizeof option2/sizeof option2[0]);
      }
      else{
        std::cout << "We will begin with the intrinsic parameter calibration of individual cameras" << std::endl;
      }

      bool saveData = true;

      switch(choice){
        case 0: {
          printf("\033[2J\033[1;H\033[?25l"); 
          std::cout << "Please wait while the intrinsic camera parameters are generated" << std::endl;
          calib->calibrationFromFiles(i,0,300);
        }; break;
        case 1: {
          calib->calibration(i,false,saveData,totalImg);    
          printf("\033[2J\033[1;H\033[?25l");     
          std::cout << "Please wait while the intrinsic camera parameters are generated" << std::endl;
          calib->calibrationFromFiles(i,0,300);
        }; break;
        case 2: {
          calib->rmFolder(i);
          calib->calibration(i,false,saveData,0);
          printf("\033[2J\033[1;H\033[?25l"); 
          std::cout << "Please wait while the intrinsic camera parameters are generated" << std::endl;
          calib->calibrationFromFiles(i,0,300);
        }; break;
        case 3: break;
      }
    }
    
}

void EyesGUI::extrinsicParam(Eyes* calib){
	ostringstream header;
	unsigned choice;

    printf("\033[2J\033[1;H\033[?25l"); 
    header.str("");
    
    unsigned totalImg = calib->checkFolder(1,400,500);
    header << "Now we will calculate the external camera parameters\n\n" 
           << "You already have " << totalImg << " calibration photos taken for each camera\n\n";
    string option2[3];
    option2[0] = "Use pictures";
    option2[1] = "Add more pictures";
    option2[2] = "Skip this step";

    choice = Menu(header.str(), option2,  sizeof option2/sizeof option2[0]);


    bool saveData = true;

    switch(choice){
      case 0: {
        std::cout << "Please wait while the extrinsic camera parameters are generated" << std::endl;
        calib->stereoCalibration(totalImg);
      }; break;
      case 1: {
        calib->calibration(1,true,saveData,400); 
        printf("\033[2J\033[1;H\033[?25l");        
        std::cout << "Please wait while the extrinsic camera parameters are generated" << std::endl;
        totalImg = calib->checkFolder(1,400,500);
        calib->stereoCalibration(totalImg);
      }; break;
      case 2: break;
    }
}

void EyesGUI::calibration(Eyes* calib){
	swapCameras(calib);
	intrinsicParam(calib);
	extrinsicParam(calib);
}

void EyesGUI::rectImage(Eyes* calib){
	printf("\033[2J\033[1;H\033[?25l"); 
	std::cout << "These are the rectified images\n\n"
	        << "Press esc to continue" << std::endl;
	calib->dispRectImage();
}