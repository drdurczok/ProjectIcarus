#include "InverseKin.h"

/*
 * IK_Thumb_A is responsible for the inverse kinematics of 
 * the motor controlling the proximal phalange of the thumb
 */
 
double calc_thumb_A(double X, double theta2, unsigned i){
  //Return angular positions of the first servo motor 
  return  acos(X/(A_5[i] + (B_5[i]*sin(theta2))));
}
 
void display_IK_Thumb(double X, double Z, double theta1, double theta2){
  Serial.print("Coordinate System Thumb Data: X_3 = ");
  Serial.print(X);
  Serial.print(" X_4 = ");
  Serial.print(Z);
  Serial.print("\t\t\t\t|  Theta1 = ");
  Serial.print(theta1*rad2deg);
  Serial.print(" deg\tTheta2 = ");
  Serial.print(theta2*rad2deg);
  Serial.print("deg\n\n");
}
