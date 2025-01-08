void advance_forward(byte motorAspeed, byte motorBspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);

  digitalWrite(MotorA_I1, HIGH);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, HIGH);
  digitalWrite(MotorB_I2, LOW);
}

void advance_right(byte motorAspeed, byte motorBspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, HIGH);
  digitalWrite(MotorB_I1, HIGH);
  digitalWrite(MotorB_I2, LOW);
}
void advance_left(byte motorAspeed, byte motorBspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);

  digitalWrite(MotorA_I1, HIGH);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, HIGH);
}

void advance_stop() {
  analogWrite(MotorA_PWMA, 0);
  analogWrite(MotorB_PWMB, 0);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, LOW);
}
void advance_back(byte motorAspeed, byte motorBspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, HIGH);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, HIGH);

}