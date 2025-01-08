/*此程式適用兩輪驅動車，若有更多輪子的需求，則照以下程式碼進行修改
使用方式
呼叫advance_forward(125,125);裡面有兩個數字,各自代表一個輪子,而數字則代表他的速度
若為四輪驅動車則可以變成advance_forward(125,125,125,125)
但需要用到這個函式，原先的函式則要修改!*/

void advance_forward(byte motorAspeed, byte motorBspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, HIGH);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, HIGH);
}


void advance_back(byte motorAspeed, byte motorBspeed) {
  analogWrite(MotorA_PWMA, motorAspeed);
  analogWrite(MotorB_PWMB, motorBspeed);

  digitalWrite(MotorA_I1, HIGH); /*與forward差別在於LOW HIGH相反，因此倒轉*/
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
  analogWrite(MotorB_PWMB, 0);
  analogWrite(MotorA_PWMA, 0);

  digitalWrite(MotorA_I1, LOW);
  digitalWrite(MotorA_I2, LOW);
  digitalWrite(MotorB_I1, LOW);
  digitalWrite(MotorB_I2, LOW);
}