#include "Wire.h"
#include <Servo.h>
#include <HX711.h>
#include <stdio.h>
#include <stdbool.h>
#include "Adafruit_NeoPixel.h"

#define FRmotor1 36 //馬達腳位
#define FRmotor2 34
#define FRmotorEn 5

#define FLmotor1 30
#define FLmotor2 32
#define FLmotorEn 4

#define RRmotor1 22
#define RRmotor2 24
#define RRmotorEn 3

#define RLmotor1 26
#define RLmotor2 28
#define RLmotorEn 2

#define led_pin 23         // WS2812B資料腳位
#define NUM_LEDS 7         // 7顆LED

// 定義伺服馬達接腳
#define SERVO_PIN1 6 
#define SERVO_PIN2 7
#define SERVO_PIN3 10
#define DT  8   // DT 腳位
#define SCK 9   // SCK 腳位
HX711 scale;
Servo servo1;
Servo servo2;
Servo servo3;

// 控制變數
int pwm_output = 0;  // 從Python接收的PWM輸出
float baseSpeed = 10;  // 基礎速度
float speedAdjustment = 0;
unsigned long motor_update_time = 0;
unsigned long last_receive_time = 0;
const unsigned long TIMEOUT_MS = 1000; // 1000ms超時
int first_rec = 0;
int target_weight;

// PID
float adj_limit = 30; // 馬達每次最大調整值
float error = 0;
float derivative = 0;
float last_error = 0;
float integral = 0;
float kp = 4;
float ki = 0.1;   
float kd = 0.25;  

// 馬達速度映射參數
const int MIN_SPEED_FR = 33;
const int MIN_SPEED_FL = 55;
const int MIN_SPEED_RR = 40;
const int MIN_SPEED_RL = 40;

// LED
Adafruit_NeoPixel strip(NUM_LEDS, led_pin, NEO_RGB + NEO_KHZ800);

void setup() {
  Wire.begin();
  Serial.begin(115200);
  // 設定馬達引腳
  // FRmotor
  pinMode(FRmotor1, OUTPUT);  
  pinMode(FRmotor2, OUTPUT);      
  // FLmotor
  pinMode(FLmotor1, OUTPUT);  
  pinMode(FLmotor2, OUTPUT); 
  // RRmotor
  pinMode(RRmotor1, OUTPUT);  
  pinMode(RRmotor2, OUTPUT); 
  // RLmotor
  pinMode(RLmotor1, OUTPUT);  
  pinMode(RLmotor2, OUTPUT);  

  // Servo
  pinMode(SERVO_PIN1, OUTPUT);
  pinMode(SERVO_PIN2, OUTPUT);
  pinMode(SERVO_PIN3, OUTPUT);
  digitalWrite(SERVO_PIN1, LOW);
  digitalWrite(SERVO_PIN2, LOW);
  digitalWrite(SERVO_PIN3, LOW);
  delay(100); // 等待穩定

  // 初始化馬達停止狀態
  stopAllMotors();

  servo1.attach(SERVO_PIN1);
  servo2.attach(SERVO_PIN2);
  servo3.attach(SERVO_PIN3);
  for (int i = 0; i < 5; i++) {
    servo1.write(40);
    servo2.write(0);
    servo3.write(80);
    delay(20);
  }

  // HX711 如果線沒接好會卡在這
  scale.begin(DT, SCK);
  scale.set_scale(430.f);     
  scale.tare();

  // LED初始化不亮燈
  strip.begin();
  strip.clear();
  colorSetting(0, 0, 0);
  
  Serial.println("Arduino PID控制系統初始化完成");
  Serial.println("等待Python程式連接...");
}

void loop() {
  servo1.write(40);
  servo2.write(0);
  servo3.write(80);
  // 檢查是否收到Python的PWM數據
  checkPythonData();
  // 檢查通信超時
  if (millis() - last_receive_time > TIMEOUT_MS) {
    stopAllMotors();
  }
  else if (millis() - motor_update_time > 0) { 
    colorSetting(0, 0, 0);
    picture_control();
    updateMotorControl();
    motor_update_time = millis();
  }
}

void checkPythonData() {
  // 檢查是否收到Python的PWM數據
  if (Serial.available() > 0) {
    String received_data = Serial.readStringUntil('\n');
    received_data.trim();
    
    if (received_data.length() > 0) {
      pwm_output = received_data.toFloat();
      last_receive_time = millis();
    }
  }
}

void updateMotorControl() {
  error = pwm_output;
  integral += error;
  derivative = error - last_error;
  last_error = error;
  integral = constrain(integral, -100, 100); // 限制積分項
  speedAdjustment = kp * error + ki * integral + kd * derivative;
  speedAdjustment = constrain(speedAdjustment, -adj_limit, adj_limit); // 限制調整值
  float speedL = constrain(baseSpeed - speedAdjustment, 0, 255); // 左輪
  float speedR = constrain(baseSpeed + speedAdjustment, 0, 255); // 右輪

  // 控制馬達（FR=右前，FL=左前，RR=右後，RL=左後）
  advance_forward1(speedR, speedL, speedR, speedL);
}

void advance_forward1(int speedFR, int speedFL, int speedRR, int speedRL) { // 輸出只有往前
  // 映射速度到有效範圍，避免馬達停轉
  speedFR = map(speedFR, 0, 255, MIN_SPEED_FR, 255);
  speedFL = map(speedFL, 0, 255, MIN_SPEED_FL, 255);
  speedRR = map(speedRR, 0, 255, MIN_SPEED_RR, 255);
  speedRL = map(speedRL, 0, 255, MIN_SPEED_RL, 255);

  // 前右馬達 (FR)
  if (speedFR > 0) {
    digitalWrite(FRmotor1, LOW); 
    digitalWrite(FRmotor2, HIGH);
    analogWrite(FRmotorEn, speedFR);
  } else {
    digitalWrite(FRmotor1, LOW); 
    digitalWrite(FRmotor2, LOW);
    analogWrite(FRmotorEn, 0);
  }
  
  // 前左馬達 (FL)
  if (speedFL > 0) {
    digitalWrite(FLmotor1, LOW); 
    digitalWrite(FLmotor2, HIGH);
    analogWrite(FLmotorEn, speedFL);
  } else {
    digitalWrite(FLmotor1, LOW); 
    digitalWrite(FLmotor2, LOW);
    analogWrite(FLmotorEn, 0);
  }
  
  // 後右馬達 (RR)
  if (speedRR > 0) {
    digitalWrite(RRmotor1, LOW); 
    digitalWrite(RRmotor2, HIGH);
    analogWrite(RRmotorEn, speedRR);
  } else {
    digitalWrite(RRmotor1, LOW); 
    digitalWrite(RRmotor2, LOW);
    analogWrite(RRmotorEn, 0);
  }
  
  // 後左馬達 (RL)
  if (speedRL > 0) {
    digitalWrite(RLmotor1, LOW); 
    digitalWrite(RLmotor2, HIGH);
    analogWrite(RLmotorEn, speedRL);
  } else {
    digitalWrite(RLmotor1, LOW); 
    digitalWrite(RLmotor2, LOW);
    analogWrite(RLmotorEn, 0);
  }
}

void stopAllMotors() {
  // 停止所有馬達
  digitalWrite(FRmotor1, LOW);
  digitalWrite(FRmotor2, LOW);
  analogWrite(FRmotorEn, 0);
  
  digitalWrite(FLmotor1, LOW);
  digitalWrite(FLmotor2, LOW);
  analogWrite(FLmotorEn, 0);
  
  digitalWrite(RRmotor1, LOW);
  digitalWrite(RRmotor2, LOW);
  analogWrite(RRmotorEn, 0);
  
  digitalWrite(RLmotor1, LOW);
  digitalWrite(RLmotor2, LOW);
  analogWrite(RLmotorEn, 0);
}

void picture_control() {
  switch (int(pwm_output)) {
    case 1111: // 雞 綠燈
      first_rec_test(1111);
      delay(2000);
      stopAllMotors();
      delay(1000);
      colorSetting(0, 255, 0);
      delay(3000);
      break;
    case 2222: // 牛 紅燈
      first_rec_test(2222);
      stopAllMotors();
      delay(1000);
      colorSetting(255, 0, 0);
      delay(3000);
      break;
    case 3333: // 豬 黃燈
      first_rec_test(3333);
      delay(2000);
      stopAllMotors();
      delay(1000);
      colorSetting(255, 255, 0);
      delay(3000);
      break;
    case 4444: // 裝飼料
      stopAllMotors();
      extend_slow(5);
      if (weight_enough(get_feed_weight())) {
        returnx_slow(5);
        delay(500);
      }
      break;
    case 5555: // 倒飼料
      stopAllMotors();
      pour_feed_slow(10);
      break;
    default:
      return;
  }
}

void colorSetting(int r, int g, int b) {
  strip.clear();
  strip.fill(strip.Color(r, g, b), 0, NUM_LEDS);
  strip.show();
}

void extend_slow(int delay_time) { // 手臂慢速伸出
  servo2.write(0);
  servo3.write(80);
  for (int i = 40; i <= 90; i++) {
    servo1.write(i);
    delay(delay_time);
  }
}

void returnx_slow(int delay_time) { // 手臂慢速收回
  servo2.write(0);
  for (int i = 90; i >= 40; i--) {
    servo1.write(i);
    delay(delay_time);
  }
  servo3.write(80);
}

void bow1_slow(int delay_time) {
  for (int i = 0; i <= 130; i += 1) {
    servo2.write(i);
    delay(delay_time);
  }
}

void bow2_slow(int delay_time) {
  for (int i = 130; i >= 0; i -= 1) {
    servo2.write(i);
    delay(delay_time);
  }
}

bool weight_enough(int target_weight) { // 檢測是否到達目標重量
  long reading = 0;
  while (true) {
    if (scale.is_ready()) {
      reading = -scale.get_units(1); // 取1次平均
    }
    if (reading >= target_weight) {
      return true;
    }
  }
}

void first_rec_test(int status) { // 儲存第一次辨識物
  if (first_rec == 0) {
    first_rec = status;
  }
}

int get_feed_weight() { // 取得目標飼料重量
  switch (first_rec) {
    case 1111:
      target_weight = 40;
      break;
    case 2222:
      target_weight = 170;
      break;
    case 3333:
      target_weight = 120;
      break;
  }
  return target_weight;
}

void pour_feed_slow(int delay_time) { // 倒飼料
  servo2.write(0);
  servo3.write(140);
  delay(500);
  for (int i = 25; i <= 120; i++) {
    servo1.write(i);
    delay(delay_time);
  }
  delay(500);
  bow1_slow(delay_time);
  delay(500);
  bow2_slow(delay_time);

  servo2.write(0);
  delay(500);
  for (int i = 120; i >= 25; i--) {
    servo1.write(i);
    delay(delay_time);
  }
  servo3.write(140);
  delay(500);
}
