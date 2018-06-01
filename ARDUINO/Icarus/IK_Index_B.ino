#include "InverseKin.h"

/*
 * IK_Index_B is responsible for the inverse kinematics of 
 * the motor controlling the medial/distal phalange
 */
 
//This system takes the point at the joint as input, ignoring finger tip
double calc_sys_3_ang(double X, double ang1, double angH){
  double ang2 = acos(X/C_3[i]) - (ang1 /*+ angH*/);
  return gamma = PI - ang2 - init_offsetC[i];
}

double calc_sys_3_len(double gamma){
  return sqrt( pow(B_2[i],2)+pow(C_3[i],2)-2*B_2[i]*C_3[i]*cos(gamma) );
}

double calc_pos_coord_2(double ang, double len){
  //Return fixed position of coordinate system
  return len*cos(ang);
}

double calc_orien_coord_2(double gamma, double len){
  //Return orientation of coordinate system
  double ang = asin( (C_3[i]*sin(gamma))/len );
  return PI - ang - init_offsetD[i];
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

