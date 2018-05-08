#include "SysProperties.h"

double calc_sys_1();
double calc_sys_2();
double calc_sys_3();
double calc_sys_4();

// System Properties


void setup() {
  // put your setup code here, to run once:
 
}

void loop() {
  //First Motor
  alpha = calc_sys_2(X_1);
  phi = calc_sys_1(alpha);

  //Second Motor
  calc_sys_3();
  calc_sys_4(X_2);
}

double calc_sys_1(double alpha){
  double E, phi;
  E = sqrt(pow(A_1,2)+pow(D_1,2)-2*A_1*D_1*cos(alpha));
  phi = asin(A_1*sin(alpha)/E) + acos((pow(C_1,2)+pow(E,2)-pow(B_1,2))/(2*C_1*E));
  
  return phi;
}

double calc_sys_2(double X){
  double alpha;
  alpha = acos( ((X-A_2*cos(init_offsetA))/B_2)-init_offsetA );
  
  return alpha;
}

double calc_sys_3(){
  double len;
//calculate gamma
  len = sqrt( pow(A_3,2)+pow(C_3,2)-2*A_3*C_3*cos(gamma) );
  return len;
}

double calc_sys_4(double X){
  return A_4*sin(ang_4)+B_4*sin(ang_4+flexion)+X;
}

