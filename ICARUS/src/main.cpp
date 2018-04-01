
#include <termios.h>
#include <thread>
#include <future>
#include <unistd.h>
#include "../include/DisparityEye.h"
#include "../include/Eyes.h"

using namespace std;

void calibration();
void disparity();
void menuThread(std::promise<unsigned> &&, string, string*, unsigned);
unsigned Menu(string, string*, unsigned);
void restore_terminal_settings(void);
void disable_waiting_for_enter(void);
static struct termios oldt; //getchar without pressing enter


int main( int argc, const char** argv)
{

  unsigned choice;
  disable_waiting_for_enter();


  string option[4];
  option[0] = "Enter Calibration Mode";
  option[1] = "Create Disparity Map";
  option[2] = "Start Finger Capture";
  option[3] = "Exit";

  bool loop = true;
  while(loop == true){
    choice = Menu("", option, sizeof option/sizeof option[0]);

    switch(choice){
      case 0: calibration(); break;
      case 1: disparity(); break;
      case 3: loop = false; break;
      default: loop = false;
    }
  }
    //Eyes sight;
    //sight.undistortImg();
    //sight.recordVideo();
    //sight.depthMap();
  printf("\033[2J\033[1;H\033[?25l");
  
  return 0;
}

void calibration(){
  Eyes calib;

  ostringstream header;
  header.str("");
  header << "Welcome to the Camera Calibration process." 
        << "\n\nConfirm that this is the left camera.\n\n";
  string option[2];
  option[0] = "Correct camera selected";
  option[1] = "Swap cameras";

  std::promise<unsigned> p;
  auto f = p.get_future();

  thread opt(menuThread, std::move(p), header.str(), option, sizeof option/sizeof option[0]);

  calib.dispCamera(1);

  opt.join();
  printf("\033[2J\033[1;H\033[?25l"); 

  unsigned choice = f.get();
  switch(choice){
    case 0: break;
    case 1: calib.swapCameras();
  }

  //Intrinsic Parameters
  for(unsigned i=1; i<3; i++){
    printf("\033[2J\033[1;H\033[?25l"); 
    header.str("");
    
    unsigned totalImg = calib.checkFolder(i,0);
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
        std::cout << "Please wait while the intrinsic camera parameters are generated" << std::endl;
        calib.calibrationFromFiles(i,0,300);
      }; break;
      case 1: {
        calib.calibration(i,i,saveData,totalImg);    
        printf("\033[2J\033[1;H\033[?25l");     
        std::cout << "Please wait while the intrinsic camera parameters are generated" << std::endl;
        calib.calibrationFromFiles(i,0,300);
      }; break;
      case 2: {
        calib.rmFolder(i);
        calib.calibration(i,i,saveData,0);
        printf("\033[2J\033[1;H\033[?25l"); 
        std::cout << "Please wait while the intrinsic camera parameters are generated" << std::endl;
        calib.calibrationFromFiles(i,0,300);
      }; break;
      case 3: break;
    }
  }
  
  //Extrinsic Parameters
  printf("\033[2J\033[1;H\033[?25l"); 
  header.str("");
  
  unsigned totalImg = calib.checkFolder(1,400);
  header << "Now we will calculate the external camera parameters\n\n" 
         << "You already have " << totalImg << " calibration photos taken for each camera\n\n";
  string option2[4];
  option2[0] = "Use pictures";
  option2[1] = "Add more pictres";
  option2[2] = "Skip this step";

  choice = Menu(header.str(), option2,  sizeof option2/sizeof option2[0]);


  bool saveData = true;

  switch(choice){
    case 0: {
      std::cout << "Please wait while the extrinsic camera parameters are generated" << std::endl;
      calib.stereoCalibration(totalImg);
    }; break;
    case 1: {
      calib.calibration(1,2,saveData,400); 
      printf("\033[2J\033[1;H\033[?25l");        
      std::cout << "Please wait while the extrinsic camera parameters are generated" << std::endl;
      totalImg = calib.checkFolder(1,400);
      calib.stereoCalibration(totalImg);
    }; break;
    case 2: break;
  }

  printf("\033[2J\033[1;H\033[?25l"); 
  std::cout << "The calibration process is over\n"
            << "These are the rectified images\n\n"
            << "Press esc to continue" << std::endl;
  calib.dispRectImage();
}

void disparity(){
  DisparityEye sight;
  sight.depthMap();
}

void restore_terminal_settings(void){
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void disable_waiting_for_enter(void){
    struct termios newt;

    /* Make terminal read 1 char at a time */
    tcgetattr(0, &oldt);  /* Save terminal settings */
    newt = oldt;  /* Init new settings */
    newt.c_lflag &= ~(ICANON | ECHO);  /* Change settings */
    tcsetattr(0, TCSANOW, &newt);  /* Apply settings */
    atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends  */
}

void menuThread(std::promise<unsigned> && p, string header, string* option, unsigned maxSize){
  unsigned temp = Menu(header,option,maxSize);
  p.set_value(temp);
  printf("\033[2J\033[1;H\033[?25l"); 
  cout << "Close the camera window!" << endl;
}

unsigned Menu(string header, string* option, unsigned maxSize){
  unsigned state = 0, choice = 0;
  bool cond = true;

  ostringstream selected;
  while(cond){
    printf("\033[2J\033[1;H\033[?25l"); //ANSI Escape codes: \033[2J - Clears the entire screen(J means to clear, 2 means the entire screen), \033[1;H - Positions the cursor row 1 column 1, \033[?25l hides the cursor (033 is ESC in octal)
    
    std::cout << header;

    for(unsigned i=0; i<maxSize; i++ ){
      if(i==state){
        selected.str("");
        selected << "\033[1;31m" << option[i] << "\033[0m"; 
        std::cout << selected.str() << std::endl;
      }
      else
        std::cout << option[i] << std::endl;
    }

    choice = getchar();

    switch(choice){
      //Arrow Up Key
      case 65: {
        if(state > 0)
          state = state-1;
        else
          state = maxSize-1;
      } break;
      //Arrow Down Key
      case 66: {
        if(state < maxSize-1)
          state = state+1;
        else
          state = 0;
      } break;
      //Enter Key
      case 10: cond = false;  
    }
  }
  return state;
}