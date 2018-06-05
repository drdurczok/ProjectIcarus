#include <Servo.h>

//First Finger Motors
Servo motor01, motor02;
//Second Finger Motors
Servo motor03, motor04;
//Thumb Motors
Servo motor05, motor06;

int servo_position = 0;

const double rad2deg = 180/PI;
const double deg2rad = PI/180;

//Motor 1
double theta,phi;
double coord_1_X,coord_1_ang;
//Motor 2
double gamma,len;
double coord_2_X,coord_2_ang;
//Thumb Motors
double theta1,theta2;

//temp input values
double X_1 = 54; //Domain [0,A_2]
double X_2 = 30; //Domain [-a,X_1+b]
double X_3 = 1;
double X_4 = 1;
double ang_H = 0 *deg2rad; //Domain [-90,90] Rad

unsigned i = 0;

bool flag = 0;

void setup() {
  Serial.begin(57600);
  motor01.attach(2);  
  motor02.attach(4);
  motor03.attach(6);  
  motor04.attach(8);
  motor05.attach(10);  
  motor06.attach(12);
}

void loop() {
/* First Motor Calculations (INVERSE)
 * X_1 - First coordinate point
 * phi - Angle of first motor
 */
  theta = calc_sys_2(X_1,ang_H,i);
  phi = calc_sys_1(theta,i);
/* Save data for first coordinate data (FORWARD)
 * This gives us the new coordinate system
 * for the next set of systems.
 */
  coord_1_ang = calc_orien_coord_1(theta,i);
  coord_1_X = calc_pos_coord_1(coord_1_ang,i);

  //display_IK_Index_A(X_1,theta,phi,coord_1_ang,coord_1_X);

  //Second Motor
  //calc_sys_4(X_2,i);
  gamma = calc_sys_3_ang(X_2,coord_1_ang,ang_H,i); //X_2,coord_1_X,ang_H
  len = calc_sys_3_len(gamma,i);

  coord_2_ang = calc_orien_coord_2(gamma,len,i);
  coord_2_X = calc_pos_coord_2(coord_2_ang,len);

  //display_IK_Index_B(X_2,gamma,len,coord_2_ang, coord_2_X);

  theta2 = calc_thumb_B(X_4,0);
  theta1 = calc_thumb_A(X_3,theta2,0);

  //display_IK_Thumb(X_3,X_4,theta1,theta2);
  //delay(500);

/*
  //motor01.write(-180);
  //delay(10);

*/
//Motor Control
  motor01.write(173-phi*rad2deg);
  motor03.write(190-phi*rad2deg);
  delay(15);


  if(X_1==54) flag = 1;
  if(X_1==0) flag = 0;
  if(flag==0)
    X_1 = X_1 + 1;
  else
    X_1 = X_1 - 1;
    
  if(X_2<45)
    X_2 = X_2 + 1;
  else
    X_2 = 0;
}


