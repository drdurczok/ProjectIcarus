#include "InverseKin.h"

/*
 * IK_Thumb_B is responsible for the inverse kinematics of 
 * the motor controlling the medial/distal phalange of the thumb
 */
 
double calc_thumb_B(double Z, unsigned i){
  //Return angular positions of the second servo motor 
  return acos(-Z/B_5[i]);
}
