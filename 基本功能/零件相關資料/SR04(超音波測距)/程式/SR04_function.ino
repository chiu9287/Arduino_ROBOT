float ping_d() { //探測距離
  digitalWrite(F_Trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(F_Trig, LOW);
  delayMicroseconds(10);
  duration = pulseIn(F_Echo, HIGH);
  distance = (duration / 2) /29.1;
  Serial.println(distance);
  return distance;
}