#include <Servo.h>

Servo motor01;

int servo_position = 0;

const double rad2deg = 180/PI;
const double deg2rad = PI/180;

double alpha,alpha2,phi;

//temp input values
double X_1 = 0; //Domain [0,A_2]
double X_2 = 1.5; //Domain [-a,X_1+b]
double ang_H = 10 *deg2rad; //Domain [-90,90] Rad


// Coordinates 1
double coord_1_X;
double coord_1_ang;

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
  Serial.print("deg  |  Ang pos of motor 1: PHI = ");
  Serial.print(phi*rad2deg);
  Serial.print("deg\n");
  delay(500);

  //Second Motor
  //calc_sys_4(X_2);
  //calc_sys_3();

  motor01.write(-180);
  delay(10);



  if(X_1<1)
    X_1 = X_1 + 0.05;
  else
    X_1 = 0;
}


