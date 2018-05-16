double alpha,phi;

//temp input values
double X_1 = 1.6;
double X_2 = 2.5;

void setup() {
  Serial.begin(57600);
}

void loop() {
  //First Motor
  alpha = calc_sys_2(X_1);
  phi = calc_sys_1(alpha);
  Serial.print(phi);
  Serial.print("\n");

  //Second Motor
  calc_sys_3();
  //calc_sys_4(X_2);
}


