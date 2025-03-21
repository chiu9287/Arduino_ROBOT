// LED light ball
const int Red = A13;
const int Green = A14;
const int Blue = A15;
const int in1=A12;

void setup() {
  pinMode(in1, OUTPUT);
  digitalWrite(in1,HIGH);
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
}

void loop() {  
  //1s紅燈
  setColor(255, 0, 255);  // 綠色
  delay(1000);
}

void setColor(int redValue, int greenValue, int blueValue) {  // for light ball
  analogWrite(Red, redValue);
  analogWrite(Green, greenValue);
  analogWrite(Blue, blueValue);
}