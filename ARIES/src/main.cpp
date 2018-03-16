#include <fstream>  
#include "../include/Listen.h"
#include "../include/Audio.h"
#include "../include/OperationBase.h"
#include "../include/UI.h"
#include "../include/Time.h"


std::string decode();
std::string action(std::string);

Listen listen;
Audio audio;
UI UIX;

std::map<std::string, OperationBase> myFunc = 		//{key, {pointer to object, member function}}
	{ 		
		{"time", {&UIX,(ClassProc)&UI::curTime}},
		{"day",{&UIX,(ClassProc)&UI::curDay}},
		{"date",{&UIX,(ClassProc)&UI::curDate}}
	};


int main()
{
  std::string outputString;
  std::string speakString;

  audio.playback("sounds/beep.wav");

  while(outputString != "shutdown")
  {
   outputString = decode(); 

   if(outputString == "explain") audio.festival("I am a beta version of a software engine that is meant to add voice functionality to other code. My advantage over siri and other such programs is that I specialize in specific vocabulary. This makes me fast, accurate and moduler.");

   else if(outputString != "shutdown") 
    {
      speakString = action(outputString);

      audio.festival(speakString);
    }
  }

  audio.festival("Thank you, and goodbye");

	/*
	 *  The above sequance is very basic and linear. Ideas to increase the responsiveness of the program
	 *  is to create a thread that is always checking for new raw files and creates new threads appropriately
	 *  to carry out the various commands.
	 *
	 *  Also to minimize processor usage, have the device enter a "sleep mode" during times of low use, such as
	 *  during sleeping or school hours, or have it only begin recording after a specific gesture, "ok".
   *
	 */


	return 0;
}


std::string decode()
{
  std::string er = "error";

	std::string fileLoc = "audio.raw"; 	//this is the location of the file we will be comparing against our langauge model

	std::string outputString;  //holds the decoded string

	if(listen.init()>0)
	{
		std::cerr<<"error on init"<<std::endl;
		return er;
	}
  
  listen.setKeyphrase();

  std::string keyword;

  while(keyword != "aries")
  {
    audio.recordAudio(2000000);
    keyword = listen.rawToString(fileLoc.c_str());
    remove ("audio.raw");
  }

  listen.unsetKeyphrase();

  audio.playback("sounds/beep.wav");

  audio.recordAudio(4000000);
  outputString = listen.rawToString(fileLoc.c_str());   //process the .raw file and store the string
  remove ("audio.raw");

  return outputString;
}


std::string action(std::string key) 
{ 
  std::string speakString;

  if ( myFunc.find(key) == myFunc.end() ) {
  speakString = "ERROR: Key not found";
  } else {
  speakString = myFunc.find(key)->second.execute(); 
  }

  return speakString;
}


