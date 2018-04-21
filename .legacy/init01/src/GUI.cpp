#include "../include/GUI.h"

GUI::GUI(){

}

void GUI::menuThread(std::promise<unsigned> && p, string header, string* option, unsigned maxSize){
  unsigned temp = Menu(header,option,maxSize);
  p.set_value(temp);
  printf("\033[2J\033[1;H\033[?25l"); 
  cout << "Close the camera window!" << endl;
}

unsigned GUI::Menu(string header, string* option, unsigned maxSize){
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