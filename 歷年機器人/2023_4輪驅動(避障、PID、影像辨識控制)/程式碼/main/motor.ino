void advance_forward(byte motorAspeed, byte motorBspeed, byte motorCspeed, byte motorDspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);
  analogWrite(MotorC_PWMC, motorCspeed);
  analogWrite(MotorD_PWMD, motorDspeed);

  digitalWrite(MotorA_I1, HIGH);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, HIGH);
  digitalWrite(MotorB_I2, LOW);

  digitalWrite(MotorC_I1, HIGH);
  digitalWrite(MotorC_I2, LOW);
  digitalWrite(MotorD_I1, HIGH);
  digitalWrite(MotorD_I2, LOW);
}

void advance_left(byte motorAspeed, byte motorBspeed, byte motorCspeed, byte motorDspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);
  analogWrite(MotorC_PWMC, motorCspeed);
  analogWrite(MotorD_PWMD, motorDspeed);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, HIGH);
  digitalWrite(MotorB_I1, HIGH);
  digitalWrite(MotorB_I2, LOW);

  digitalWrite(MotorC_I1, LOW);
  digitalWrite(MotorC_I2, HIGH);
  digitalWrite(MotorD_I1, HIGH);
  digitalWrite(MotorD_I2, LOW);
}
void advance_right(byte motorAspeed, byte motorBspeed, byte motorCspeed, byte motorDspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);
  analogWrite(MotorC_PWMC, motorCspeed);
  analogWrite(MotorD_PWMD, motorDspeed);

  digitalWrite(MotorA_I1, HIGH);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, HIGH);

  digitalWrite(MotorC_I1, HIGH);
  digitalWrite(MotorC_I2, LOW);
  digitalWrite(MotorD_I1, LOW);
  digitalWrite(MotorD_I2, HIGH);
}

void advance_stop() {
  analogWrite(MotorA_PWMA, 0);
  analogWrite(MotorB_PWMB, 0);
  analogWrite(MotorC_PWMC, 0);
  analogWrite(MotorD_PWMD, 0);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, LOW);
  digitalWrite(MotorC_I1, LOW);
  digitalWrite(MotorC_I2, LOW);
  digitalWrite(MotorD_I1, LOW);
  digitalWrite(MotorD_I2, LOW);
}
void advance_back(byte motorAspeed, byte motorBspeed, byte motorCspeed, byte motorDspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);
  analogWrite(MotorC_PWMC, motorCspeed);
  analogWrite(MotorD_PWMD, motorDspeed);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, HIGH);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, HIGH);

  digitalWrite(MotorC_I1, LOW);
  digitalWrite(MotorC_I2, HIGH);
  digitalWrite(MotorD_I1, LOW);
  digitalWrite(MotorD_I2, HIGH);
}
