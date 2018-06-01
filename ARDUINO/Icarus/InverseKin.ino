#include "InverseKin.h"


//_______Additional Formulas_______
double triangle1(double side1, double side2, double ang){
  return sqrt(pow(side1,2)+pow(side2,2)-2*side1*side2*cos(ang));
}

