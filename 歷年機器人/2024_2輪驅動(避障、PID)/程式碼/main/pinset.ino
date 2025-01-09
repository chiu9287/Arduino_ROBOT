void pinset() {
  Serial.begin(115200);
  Serial2.begin(115200); //WT61C

  pinMode(MotorA_I1, OUTPUT); /*馬達設置*/
  pinMode(MotorA_I2, OUTPUT);
  pinMode(MotorB_I1, OUTPUT);
  pinMode(MotorB_I2, OUTPUT);
  pinMode(MotorA_PWMA, OUTPUT);
  pinMode(MotorB_PWMB, OUTPUT);


  pinMode(F_Trig, OUTPUT); /*前方超音波設置*/
  pinMode(F_Echo, INPUT);
  d_servo.attach(d_servopin); /*前方超音波下伺服馬達設置*/
  d_servo.write(90);          
  delay(500);

  while (true) {
    REset();
    checkMPU();
    if (ANG == 0 && roll == 0 && pitch == 0) break;
  }

}
