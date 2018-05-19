#include "InverseKin.h"

double calc_sys_1(double alpha){
  double E, phi;
  E = triangle1(A_1,D_1,alpha);
  phi = asin(A_1*sin(alpha)/E) + acos((pow(C_1,2)+pow(E,2)-pow(B_1,2))/(2*C_1*E));
  
  return phi;
}

//This system considers the orientation of the palm
double calc_sys_2(double X, double ang_H){
  double r = sqrt( pow(A_2,2) + pow((A_2-X),2) - pow(X,2) );
  double ang = acos( (pow(r,2)/(2*pow(A_2,2))-1) );
  return (PI - ang) - (ang_H + init_offsetA);
}
/*
double calc_sys_2(double X){
  //radians
  return acos( ((X-D_1*cos(init_offsetA))/A_2)-init_offsetA );
}
*/
double relsys1sys2(double ang){
  return PI - init_offsetB + ang;
}

double calc_pos_coord_1(double ang){
  //Return fixed position of coordinate system
  return A_2*cos(ang);
}

double calc_orien_coord_1(double ang){
  //Return orientation of coordinate system
  return ang + init_offsetA;
}

double calc_sys_3(){
  double len;
  double gamma;
//calculate gamma
  len = sqrt( pow(B_2,2)+pow(C_3,2)-2*B_2*C_3*cos(gamma) );
  return len;
}

double calc_sys_4(double X){
  return flexion - acos((X-D_3*cos(flexion))/A_4);
}


//_______Additional Formulas_______
double triangle1(double side1, double side2, double ang){
  return sqrt(pow(side1,2)+pow(side2,2)-2*side1*side2*cos(ang));
}

