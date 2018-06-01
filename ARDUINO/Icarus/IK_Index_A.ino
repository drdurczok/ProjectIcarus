#include "InverseKin.h"

/*
 * IK_Index_A is responsible for the inverse kinematics of 
 * the motor controlling the proximal phalange
 */

double calc_sys_1(double alpha, unsigned i){
  double E, phi, alpha2;

  // We need to transfom alpha to fit our new system with function rel_sys1_sys2
  alpha2 = rel_sys1_sys2(alpha,i);
  E = triangle1(A_1[i],D_1[i],alpha2);
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

void display_IK_Index(double X_1, double alpha, double phi){
/* Save data for first coordinate data (FORWARD)
 * This gives us the new coordinate system
 * for the next set of systems.
 */
  coord_1_ang = calc_orien_coord_1(alpha,i);
  coord_1_X = calc_pos_coord_1(coord_1_ang,i);

  Serial.print("Coordinate System 1 Data: X_1 = ");
  Serial.print(X_1);
  Serial.print(" (Corr X_1 = ");
  Serial.print(coord_1_X);
  Serial.print(") Orien = ");
  Serial.print(coord_1_ang*rad2deg);
  Serial.print("deg    \t|  Ang pos of motor 1: PHI = ");
  Serial.print(phi*rad2deg);
  Serial.print("deg\t");
  Serial.print("(alpha ");
  Serial.print(alpha*rad2deg);
  Serial.print(")\n");
}

