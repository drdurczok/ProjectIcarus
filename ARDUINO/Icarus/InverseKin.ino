#include "InverseKin.h"

double calc_sys_1(double alpha){
  double E, phi;
  E = sqrt(pow(A_1,2)+pow(D_1,2)-2*A_1*D_1*cos(alpha));
  phi = asin(A_1*sin(alpha)/E) + acos((pow(C_1,2)+pow(E,2)-pow(B_1,2))/(2*C_1*E));
  
  return phi;
}

double calc_sys_2(double X){
  double alpha;
  alpha = acos( ((X-D_1*cos(init_offsetA))/A_2)-init_offsetA );
  //radians

  return alpha;
}

double calc_sys_3(){
  double len;
  double gamma;
//calculate gamma
  len = sqrt( pow(B_2,2)+pow(C_3,2)-2*B_2*C_3*cos(gamma) );
  return len;
}

double calc_sys_4(double X){
  return A_4*sin(ang_4)+D_3*sin(ang_4+flexion)+X;
}

