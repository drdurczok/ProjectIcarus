#ifndef UI_H
#define UI_H

#include "Time.h"
#include "Dictionary.h"



class UI : public Time
{
public:
    std::string curTime();
    std::string curDay();
    std::string curDate();

private:
	std::string minute();
    std::string hour();
    std::string day();
    std::string nday();
    std::string month();
    std::string year();

};

#endif