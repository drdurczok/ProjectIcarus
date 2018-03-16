#ifndef TIME_H
#define TIME_H

#include <iostream>
#include <string>
#include <ctime>

class Time 
{
protected:
    int minute();
    int hour();
    int day();
    int nday();
    int month();
    int year();
//    stopwatch();

    time_t currentTime;
    struct tm *localTime;


};

#endif
