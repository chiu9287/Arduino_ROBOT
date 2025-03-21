// 定義L298N模塊控制馬達的引腳（前輪馬達）
const int ENA1 = 6;  // 馬達1速度控制
const int IN1 = 7;   // 馬達1轉向控制
const int IN2 = 8;  // 馬達1轉向控制
const int IN3 = 9;  // 馬達2轉向控制
const int IN4 = 10; // 馬2轉向控制
const int ENB1 = 11;  // 馬達2速度控制

// 定義L298N模塊控制馬達的引腳（後輪馬達）
const int ENA2 = A0; // 馬達3速度控制
const int IN5 = A1; // 馬達3轉向控制
const int IN6 = A2; // 馬達3轉向控制
const int IN7 = A3; // 馬達4轉向控制
const int IN8 = A4; // 馬達4轉向控制
const int ENB2 = A5; // 馬達4速度控制

void setup() {
  // put your setup code here, to run once:
  pinMode(ENA1, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB1, OUTPUT);

  pinMode(ENA2, OUTPUT);
  pinMode(IN5, OUTPUT);
  pinMode(IN6, OUTPUT);
  pinMode(IN7, OUTPUT);
  pinMode(IN8, OUTPUT);
  pinMode(ENB2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW); 
      analogWrite(ENA1, 255);
      analogWrite(ENB1, 255);

      digitalWrite(IN5, HIGH);
      digitalWrite(IN6, LOW);
      digitalWrite(IN7, HIGH);
      digitalWrite(IN8, LOW);
      analogWrite(ENA2, 255);
      analogWrite(ENB2, 255);
}
