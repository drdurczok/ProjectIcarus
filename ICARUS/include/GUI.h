#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <future>
#include <unistd.h>
#include <string>

using namespace std;

class GUI
{
    public:
        GUI();
        void menuThread(std::promise<unsigned> &&, string, string*, unsigned);
        unsigned Menu(string, string*, unsigned);

    private:

};

#endif // GUI_H
