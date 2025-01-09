float ping_d() { /*探測距離*/
  digitalWrite(F_Trig, HIGH);
  delayMicroseconds(2);
  digitalWrite(F_Trig, LOW);
  float distance = pulseIn(F_Echo, HIGH, 6000);
  distance = (distance / 2) * 0.0347;
  return distance;
}
