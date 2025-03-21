int trigPin = 40;    // 超音波感測器 Trig腳接 Arduino pin 11
int echoPin = 41;    //超音波感測器 Echo 腳接 Arduino pin 12
long duration, cm;

void setup() {
  Serial.begin (9600);           //設定序列埠監控視窗 (Serial Monitor) 和 Arduino資料傳輸速率為 9600 bps (Bits Per Second)
  pinMode(trigPin, OUTPUT);      //Arduino 對外啟動距離感測器Trig腳，射出超音波 
  pinMode(echoPin, INPUT);       //超音波被障礙物反射後，Arduino讀取感測器Echo腳的時間差
}
 
void loop()
{                                  //程式計算出距離值 cm
  delay(500);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(50);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(100);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  cm = (duration/2) / 29.1;  

  Serial.print(cm);     //印出距離值 cm 在序列埠監控顯示器 單位公分
  Serial.println(" cm");
  }
