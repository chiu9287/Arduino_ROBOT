const int IN1 = A12;  // 共陽
const int G = A13;
const int R = A14;
const int B = A15;


void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  digitalWrite(IN1, HIGH);

}
void loop() {
  analogWrite(R, 0);     // 紅色通道 (共陽極)
  analogWrite(G, 255); // 綠色通道 (共陽極)
  analogWrite(B, 255);
  delay(500);
  analogWrite(R, 255);     // 紅色通道 (共陽極)
  analogWrite(G, 0); // 綠色通道 (共陽極)
  analogWrite(B, 255);
  delay(500);
  analogWrite(R, 255);     // 紅色通道 (共陽極)
  analogWrite(G, 255); // 綠色通道 (共陽極)
  analogWrite(B, 0);
  delay(500);
}
