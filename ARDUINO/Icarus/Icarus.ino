#include <Servo.h>

Servo motor01;

int servo_position = 0;

const double rad2deg = 180/PI;
const double deg2rad = PI/180;

//Motor 1
double alpha,alpha2,phi;
double coord_1_X,coord_1_ang;
//Motor 2
double gamma,len;
double coord_2_X,coord_2_ang;

//temp input values
double X_1 = 0; //Domain [0,A_2]
double X_2 = 0.5; //Domain [-a,X_1+b]
double ang_H = 10 *deg2rad; //Domain [-90,90] Rad


void setup() {
  Serial.begin(57600);
  motor01.attach(9);
}

void loop() {
/* First Motor Calculations (INVERSE)
 * X_1 - First coordinate point
 * phi - Angle of first motor
 */
  alpha = calc_sys_2(X_1,ang_H);
  phi = calc_sys_1(alpha);

/* Save data for first coordinate data (FORWARD)
 * This gives us the new coordinate system
 * for the next set of systems.
 */
  coord_1_ang = calc_orien_coord_1(alpha);
  coord_1_X = calc_pos_coord_1(coord_1_ang);

  Serial.print("Coordinate System 1 Data: X_1 = ");
  Serial.print(X_1);
  Serial.print(" (Corr X_1 = ");
  Serial.print(coord_1_X);
  Serial.print(") Orien = ");
  Serial.print(coord_1_ang*rad2deg);
  Serial.print("deg\t|  Ang pos of motor 1: PHI = ");
  Serial.print(phi*rad2deg);
  Serial.print("deg\n");
  delay(500);

  //Second Motor
  //calc_sys_4(X_2);
  gamma = calc_sys_3_ang(X_2,coord_1_X,ang_H); //X_2,coord_1_X,ang_H
  len = calc_sys_3_len(gamma);

  coord_2_ang = calc_orien_coord_2(gamma,len);
  coord_2_X = calc_pos_coord_2(coord_2_ang,len);

  Serial.print("Coordinate System 2 Data: X_2 = ");
  Serial.print(X_2);
  Serial.print(" (Corr X_2 = ");
  Serial.print(coord_2_X);
  Serial.print(") Orien = ");
  Serial.print(coord_2_ang*rad2deg);
  Serial.print("deg\t|  Len of string: L = ");
  Serial.print(len);
  Serial.print("mm");
  Serial.print(" (gamma = ");
  Serial.print(gamma*rad2deg);
  Serial.print("deg)\n\n");
  delay(500);
  
  
  motor01.write(phi*rad2deg);
  delay(10);




  if(X_1<1)
    X_1 = X_1 + 0.05;
  else
    X_1 = 0;
    
  if(X_2<1)
    X_2 = X_2 + 0.1;
  else
    X_2 = 0;
}


