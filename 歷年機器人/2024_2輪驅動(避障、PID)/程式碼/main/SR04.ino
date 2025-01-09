/*探測距離*/
float ping_d() { 
  digitalWrite(F_Trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(F_Trig, LOW);
  float distance = pulseIn(F_Echo, HIGH, 6000);
  distance = (distance / 2) * 0.0347;
  // Serial.println(distance); //此程式碼可以印出該超音波之探測距離
  return distance;
}
