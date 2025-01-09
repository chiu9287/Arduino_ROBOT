void pinset() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(MotorA_I1, OUTPUT); /*馬達設置*/
  pinMode(MotorA_I2, OUTPUT);
  pinMode(MotorB_I1, OUTPUT);
  pinMode(MotorB_I2, OUTPUT);
  pinMode(MotorA_PWMA, OUTPUT);
  pinMode(MotorB_PWMB, OUTPUT);
  pinMode(MotorC_I1, OUTPUT);
  pinMode(MotorC_I2, OUTPUT);
  pinMode(MotorD_I1, OUTPUT);
  pinMode(MotorD_I2, OUTPUT);
  pinMode(MotorC_PWMC, OUTPUT);
  pinMode(MotorD_PWMD, OUTPUT);

  pinMode(F_Trig, OUTPUT); /*前方超音波設置*/
  pinMode(F_Echo, INPUT);
  d_servo.attach(d_servopin); /*前方超音波下伺服馬達設置*/
  d_servo.write(90);          /*伺服馬達轉正90or轉165*/
  delay(500);

  pinMode(8, OUTPUT); /*繼電器設置*/
  digitalWrite(8, HIGH);
  servo1.attach(servo1_pin); /* 夾爪(10閉 80開)*/
  servo2.attach(servo2_pin); /* 夾手臂(0上 95下)*/
  servo1.write(10);
  servo2.write(0);
  delay(500);
  while (true) {
    REset();
    checkMPU();
    if (ANG == 0 && roll == 0 && pitch == 0) break;
  }
}
