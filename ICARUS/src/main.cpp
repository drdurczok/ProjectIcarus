
#include <termios.h>
#include "../include/DisparityEye.h"
#include "../include/GUI.h"
#include "../include/Tracking.h"

using namespace std;

void disparity();
void restore_terminal_settings(void);
void disable_waiting_for_enter(void);
static struct termios oldt; //getchar without pressing enter

int main( int argc, const char** argv)
{
  GUI gui;
  Tracking track;
  
  unsigned choice;
  disable_waiting_for_enter();

  string option[3];
  option[0] = "Disparity Mapping";
  option[1] = "Tracking";
  option[2] = "Exit";

  bool loop = true;
  while(loop == true){
    choice = gui.Menu("", option, sizeof option/sizeof option[0]);
        printf("a");

    switch(choice){
      case 0: disparity(); break;
      case 1: track.ScanFingerTips(); break;
      case 2: loop = false; break;
      default: loop = false;
    }
  }

  printf("\033[2J\033[1;H\033[?25l");
  
  return 0;
}


void disparity(){
  DisparityEye sight;
  sight.GUI();
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
