#include "../include/Time.h"


int Time::minute() { return localTime->tm_min; }

int Time::hour() { return localTime->tm_hour; }

int Time::day() { return (localTime->tm_mday)%7; }

int Time::nday() { return localTime->tm_mday; }

int Time::month() { return localTime->tm_mon; }

int Time::year() { return (localTime->tm_year + 1900); }
