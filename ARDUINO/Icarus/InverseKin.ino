#include "InverseKin.h"

double calc_sys_1(double alpha){
  double E, phi, alpha2;

  // We need to transfom alpha to fit our new system
  alpha2 = relsys1sys2(alpha);
  E = triangle1(A_1,D_1,alpha2);
  phi = asin(A_1*sin(alpha)/E) + acos((pow(C_1,2)+pow(E,2)-pow(B_1,2))/(2*C_1*E));
  
  return phi;
}

//This system considers the orientation of the palm and motor angle offset
double calc_sys_2(double X, double ang_H){
  return acos( X/A_2 ) - (ang_H + init_offsetA);
}

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

//This system takes the point at the joint as input, ignoring finger tip
double calc_sys_3_ang(double X, double ang1, double angH){
  double ang2 = acos(X/C_3) - (ang1 + angH);
  return gamma = PI - ang2 - init_offsetC;
}

double calc_sys_3_len(double gamma){
  return sqrt( pow(B_2,2)+pow(C_3,2)-2*B_2*C_3*cos(gamma) );
}

double calc_pos_coord_2(double ang, double len){
  //Return fixed position of coordinate system
  return len*cos(ang);
}

double calc_orien_coord_2(double gamma, double len){
  //Return orientation of coordinate system
  double ang = asin( (C_3*sin(gamma))/len );
  return PI - ang - init_offsetD;
}


/*
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
*/

//_______Additional Formulas_______
double triangle1(double side1, double side2, double ang){
  return sqrt(pow(side1,2)+pow(side2,2)-2*side1*side2*cos(ang));
}

