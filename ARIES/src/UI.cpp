#include "../include/UI.h"

std::string UI::curTime()
{
    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    std::string output;
    output = hour() + " " + minute();

    return output;
}

std::string UI::curDay()
{
    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    std::string output;
    output = "today is " + day();

    return output;
}

std::string UI::curDate()
{
    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    std::string output; 
    output = "it is the " + nday() + " of " + month();

    return output;
}

std::string UI::minute()
{
    int min = Time::minute();
    std::string sMin;

    if((min > 24) && ((min/10) < 6))
    {
        int a, b;
        a = (min/10);
        b = (min - (a*10));
        sMin = Dictionary::map_cardinal[a*10] + " " + Dictionary::map_cardinal[b];
    }
    else
    {
        sMin = Dictionary::map_cardinal[min];
    }

    return sMin;
}

std::string UI::hour()
{
    int hour = Time::hour();
    std::string sHour = Dictionary::map_cardinal[hour];

    return sHour;
}

std::string UI::day()
{
    int day = Time::day();   
    std::string sDay = Dictionary::map_day[day];

    return sDay;
}

std::string UI::nday()
{
    int day = Time::nday();
    std::string nDay;

    if((day > 24) && (day != 30))
    {
        int a, b;
        a = ((day/10)*10);
        b = (day - a);
        nDay = Dictionary::map_cardinal[a] + " " + Dictionary::map_ordinal[b];
    }
    else
        nDay = Dictionary::map_ordinal[day];

    return nDay;
}

std::string UI::month()
{
    int month = Time::month();
    std::string sMonth = Dictionary::map_month[month];

    return sMonth;
}

std::string UI::year()
{
    int year = Time::year();
    std::string sYear = Dictionary::map_cardinal[2000] + " and " + Dictionary::map_cardinal[year-2000];
    
    return sYear;
}
