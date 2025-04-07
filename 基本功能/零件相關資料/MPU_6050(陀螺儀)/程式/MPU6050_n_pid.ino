#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

unsigned long timer = 0;
float target = 0; // 設置目標角度
float error = 0;
float integral = 0;
float derivative = 0;
float last_error = 0;
float angle = 0;

// 定義L298N模塊控制馬達的引腳（前輪馬達）
const int ENA1 = 6;  // 馬達1速度控制
const int IN1 = 7;   // 馬達1轉向控制
const int IN2 = 8;  // 馬達1轉向控制
const int IN3 = 9;  // 馬達2轉向控制
const int IN4 = 10; // 馬達2轉向控制
const int ENB1 = 11;  // 馬達2速度控制

// 定義L298N模塊控制馬達的引腳（後輪馬達）
const int ENA2 = A0; // 馬達3速度控制
const int IN5 = A1; // 馬達3轉向控制
const int IN6 = A2; // 馬達3轉向控制
const int IN7 = A3; // 馬達4轉向控制
const int IN8 = A4; // 馬達4轉向控制
const int ENB2 = A5; // 馬達4速度控制

int motorSpeed = 200; // 馬達基礎速度

float Kp = 70; // 比例增益 70
float Ki = 15; // 積分增益 15
float Kd = 30; // 微分增益 30

void setup() {
  Serial.begin(9600); // 鮑率可調
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print("MPU6050 狀態: ");
  Serial.println(status);
  while(status != 0) {
    Serial.println("MPU6050 初始化失敗，請檢查連接");
    delay(1000); // 延遲一秒後重試
    status = mpu.begin();
  }
  
  Serial.println("勿觸碰陀螺儀，計算中");
  mpu.upsideDownMounting = true; // 若陀螺儀為翻轉狀態放車上或附著於車底下則取消註解此行
  delay(2000);
  mpu.calcOffsets();

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
  if((millis() - timer) > 10){ // 每10ms執行一次
    mpu.update(); // 更新陀螺儀
    pid(); // 執行PID控制
    print_angles(); // 印出角度
    timer = millis();
  }
}
void pid() {
  error = target - mpu.getAngleZ(); // 獲取yaw角度的誤差
  integral += error; // 積分
  // integral = constrain(integral, -100, 100); // 限制積分範圍，避免積分飽和
  derivative = error - last_error; // 微分
  angle = (error * Kp) + (integral * Ki) + (derivative * Kd); // 計算PID輸出

  // 限制PID輸出的範圍
  angle = constrain(angle, -255, 255);
  advance_forward(motorSpeed + angle, motorSpeed - angle, motorSpeed + angle, motorSpeed - angle); // 調整輪速(左+右-)
  last_error = error; // 更新上一次誤差
}

void advance_forward(int speedA, int speedB, int speedC, int speedD) {
  speedA = map(constrain(speedA, 0, 255), 0, 255, 30, 255); // 限制速度範圍
  speedB = map(constrain(speedB, 0, 255), 0, 255, 40, 255); // 限制速度範圍
  speedC = map(constrain(speedC, 0, 255), 0, 255, 127, 255); // 限制速度範圍
  speedD = map(constrain(speedD, 0, 255), 0, 255, 127, 255); // 限制速度範圍

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA1, speedA);
  analogWrite(ENB2, speedC);

  digitalWrite(IN5, HIGH);
  digitalWrite(IN6, LOW);
  digitalWrite(IN7, HIGH);
  digitalWrite(IN8, LOW);
  analogWrite(ENB1, speedB);
  analogWrite(ENA2, speedD);  
}

void print_angles() {
  Serial.print("  yaw(Z) : "); // 水平角度，若單純想走直線PID用此角度即可
  Serial.println(mpu.getAngleZ());
}
