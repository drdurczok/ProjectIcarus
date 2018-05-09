double alpha,phi;

//temp input values
double X_1 = 5;
double X_2 = 10;

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


