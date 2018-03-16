#ifndef DICTIONARY_H
#define DICTIONARY_H


#include <iostream>
#include <string>
#include <map>

class Dictionary
{
public:	
	friend class UI;
	friend class Time;

	static std::map<int, std::string> map_cardinal;
      static std::map<int, std::string> map_ordinal;
      static std::map<int, std::string> map_day;
      static std::map<int, std::string> map_month;
            
};

#endif 
