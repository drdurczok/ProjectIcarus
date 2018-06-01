#include "InverseKin.h"

/*
 * IK_Index_A is responsible for the inverse kinematics of 
 * the motor controlling the proximal phalange
 */

double calc_sys_1(double theta, unsigned i){
  double E, phi, alpha;

  // We need to transfom alpha to fit our new system with function rel_sys1_sys2
  alpha = rel_sys1_sys2(theta,i);
  E = triangle1(A_1[i],D_1[i],alpha);
  phi = asin(A_1[i]*sin(alpha)/E) + acos((pow(C_1[i],2)+pow(E,2)-pow(B_1[i],2))/(2*C_1[i]*E));
  
  return phi;
}

//This system considers the orientation of the palm and motor angle offset
double calc_sys_2(double X, double ang_H, unsigned i){
  return  acos( X/A_2[i] ) - (/*ang_H +*/ init_offsetA[i]);
}


/* System 2, two situations
 *  
 *        *           *                                     *           
 *          *         *                                       *         
 *            *       *                                         *       
 *              *     *                                           *     
 *                * init_offsetB                    init_offsetB(   *  )alpha
 *          theta(   * *  )alpha                          *   *   *   * * * * * * * * *
 *        *   *   *   * * * * * * * * * *                  theta(  *
 *                                                             *
 *                                                          *
 *  Theta is already calculated negative for the frst situation, therefore the following
 *  function works for both scenarios
 */ 
double rel_sys1_sys2(double theta, unsigned i){
  return PI + theta - init_offsetB[i];
}

//Corrects input X coordinate
double calc_pos_coord_1(double ang, unsigned i){
  return A_2[i]*cos(ang);
}

//Returns the orientation of the proximal phalange to zero coordinate frame
double calc_orien_coord_1(double ang, unsigned i){
  //Return orientation of coordinate system
  return ang + init_offsetA[i];
}

void display_IK_Index_A(double X_1, double theta, double phi, double coord_1_ang, double coord_1_X){
  Serial.print("Coordinate System 1 Data: X_1 = ");
  Serial.print(X_1);
  Serial.print(" (Corr X_1 = ");
  Serial.print(coord_1_X);
  Serial.print(") Orien = ");
  Serial.print(coord_1_ang*rad2deg);
  Serial.print("deg    \t|  Ang pos of motor 1: PHI = ");
  Serial.print(phi*rad2deg);
  Serial.print("deg\t");
  Serial.print("(theta ");
  Serial.print(theta*rad2deg);
  Serial.print(")\n");
}

