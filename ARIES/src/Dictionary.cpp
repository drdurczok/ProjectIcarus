#include "../include/Dictionary.h"

std::map<int, std::string> Dictionary::map_cardinal =
     {{ 1, "one"},
      { 2, "two"},
      { 3, "three"},
      { 4, "four"},
      { 5, "five"},
      { 6, "six"},
      { 7, "seven"},
      { 8, "eight"},
      { 9, "nine"},
      { 10, "ten"},
      { 11, "eleven"},
      { 12, "twelve"},
      { 13, "thirteen"},
      { 14, "fourteen"},
      { 15, "fifteen"},
      { 16, "sixteen"},
      { 17, "seventeen"},
      { 18, "eighteen"},
      { 19, "nineteen"},
      { 20, "twenty"},
      { 21, "twenty one"},
      { 22, "twenty two"},
      { 23, "twenty three"},
      { 24, "twenty four"},
      { 30, "thirty"},
      { 40, "forty"},
      { 50, "fifty"},
      { 1000, "thousand"},
      { 2000, "two thousand"}
     };

std::map<int, std::string> Dictionary::map_ordinal =
     {{ 1, "first"},
      { 2, "second"},
      { 3, "third"},
      { 4, "fourth"},
      { 5, "fifth"},
      { 6, "sixth"},
      { 7, "seventh"},
      { 8, "eighth"},
      { 9, "ninth"},
      { 10, "tenth"},
      { 11, "eleventh"},
      { 12, "twelfth"},
      { 13, "thirteenth"},
      { 14, "fourteenth"},
      { 15, "fifteenth"},
      { 16, "sixteenth"},
      { 17, "seventeenth"},
      { 18, "eighteenth"},
      { 19, "nineteenth"},
      { 20, "twentieth"},
      { 30, "thirtieth"},
     };

std::map<int, std::string> Dictionary::map_day =
      {{2, "thursday"}, 
       {3, "friday"}, 
       {4, "saturday"},
       {5, "sunday"},
       {6, "monday"},   //6
       {0, "tuesday"},
       {1, "wednesday"} //1
      };

std::map<int, std::string> Dictionary::map_month =
      {{0, "january"},
       {1, "february"},
       {2, "march"},
       {3, "april"},
       {4, "may"},
       {5, "june"},
       {6, "july"},
       {7, "august"},
       {8, "september"},
       {9, "october"},
       {10, "november"},
       {11, "december"}
      };

