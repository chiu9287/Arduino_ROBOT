#include "Wire.h"
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
#define NUM_LEDS 7     // 7顆LED

// 控制變數
float pwm_output = 0;  // 從Python接收的PWM輸出
float baseSpeed = 40;  // 基礎速度
float turnDelay =250; // 轉向延遲毫秒數
float speedAdjustment = 0;
unsigned long motor_update_time = 0;
unsigned long last_receive_time = 0;
const unsigned long TIMEOUT_MS = 500; // 500ms超時
bool pipe_exist = false;

// PID
float adj_limit = 150; // 馬達每次最大調整值
float error = 0;
float derivative = 0;
float last_error = 0;
float integral = 0;
float kp = 10;
float ki = 0.1;
float kd = 0.25;   

// 馬達速度映射參數
const int MIN_SPEED_FR = 33;
const int MIN_SPEED_FL = 55;
const int MIN_SPEED_RR = 40;
const int MIN_SPEED_RL = 40;

// 陀螺儀
unsigned char dataBuffer[11];
unsigned char dataCounter = 0;
unsigned char dataReady = 0;
float pitch = 0; // 仰角(上負下正)
int angle = 5; //檢測角度
unsigned long gyro_lastTime = 0;

// LED
int norm_light_level = 80;
int light_status;
Adafruit_NeoPixel strip(NUM_LEDS, led_pin, NEO_RGB + NEO_KHZ800);

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial2.begin(115200);
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

  // 初始化馬達停止狀態
  stopAllMotors();
  
  //WT61C陀螺儀初始化
  resetGyro();

  // LED初始化尾燈亮起
  strip.begin();
  norm_red();
  
  Serial.println("Arduino PID控制系統初始化完成");
  Serial.println("等待Python程式連接...");
}

void loop() { 
  // 檢查是否收到Python的PWM數據
  checkPythonData();
  checkGyroData();
  // 檢查通信超時
  if (millis() - last_receive_time > TIMEOUT_MS) {
    stopAllMotors();
  }
  else if (millis() - motor_update_time > 0) { 
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
  if (abs(pwm_output) == 333 || abs(pwm_output) == 666) { // pid歸零
    error = 0; last_error = 0; integral = 0; derivative = 0;
  }

  if (pwm_output == 666) { // 輸入等於666, 左或右邊值為負, 大轉彎
    stopAllMotors();
    delay(100);
    advance_forward3(200, -50, 200, -50);
    turn_signal();
    delay(turnDelay);
    return;
  } else if (pwm_output == -666) {
    stopAllMotors();
    delay(100);
    advance_forward3(-50, 200, -10, 200);
    turn_signal();
    delay(turnDelay);
    return;
  } else if (pwm_output == 333) { // 輸入等於333, 左或右邊值為0, 小轉彎
    // stopAllMotors();
    // delay(200);
    advance_forward3(100, 10, 100, 10);
    turn_signal();
    delay(turnDelay);
    return;
  } else if (pwm_output == -333) {
    // stopAllMotors();
    // delay(200);
    advance_forward3(10, 100, 10, 100);
    turn_signal();
    delay(turnDelay);
    return;
  }
  // 恢復正常尾燈
  norm_red();

  // 取得仰角並檢測斜坡並更改馬達數值
  if (angle_detect(angle)) {
    return;
  }

  // 水管檢測
  if (pwm_output == 999) { //基礎速度鎖在固定值
    pipe_exist = true;
    baseSpeed = 200;
  }
  
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

void advance_forward1(float speedFR, float speedFL, float speedRR, float speedRL) { // 輸出只有往前
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

void advance_forward2(float speedFR, float speedFL, float speedRR, float speedRL) { // 輸出只有往後  
  // 映射速度到有效範圍，避免馬達停轉
  speedFR = (speedFR < 0) ? map(-speedFR, 0, 255, MIN_SPEED_FR, 255) : 0;
  speedFL = (speedFL < 0) ? map(-speedFL, 0, 255, MIN_SPEED_FL, 255) : 0;
  speedRR = (speedRR < 0) ? map(-speedRR, 0, 255, MIN_SPEED_RR, 255) : 0;
  speedRL = (speedRL < 0) ? map(-speedRL, 0, 255, MIN_SPEED_RL, 255) : 0;

  // 前右馬達 (FR)
  if (speedFR > 0) {
    digitalWrite(FRmotor1, HIGH); 
    digitalWrite(FRmotor2, LOW);
    analogWrite(FRmotorEn, speedFR);
  } else {
    digitalWrite(FRmotor1, LOW); 
    digitalWrite(FRmotor2, LOW);
    analogWrite(FRmotorEn, 0);
  }
  
  // 前左馬達 (FL)
  if (speedFL > 0) {
    digitalWrite(FLmotor1, HIGH); 
    digitalWrite(FLmotor2, LOW);
    analogWrite(FLmotorEn, speedFL);
  } else {
    digitalWrite(FLmotor1, LOW); 
    digitalWrite(FLmotor2, LOW);
    analogWrite(FLmotorEn, 0);
  }
  
  // 後右馬達 (RR)
  if (speedRR > 0) {
    digitalWrite(RRmotor1, HIGH); 
    digitalWrite(RRmotor2, LOW);
    analogWrite(RRmotorEn, speedRR);
  } else {
    digitalWrite(RRmotor1, LOW); 
    digitalWrite(RRmotor2, LOW);
    analogWrite(RRmotorEn, 0);
  }
  
  // 後左馬達 (RL)
  if (speedRL > 0) {
    digitalWrite(RLmotor1, HIGH); 
    digitalWrite(RLmotor2, LOW);
    analogWrite(RLmotorEn, speedRL);
  } else {
    digitalWrite(RLmotor1, LOW); 
    digitalWrite(RLmotor2, LOW);
    analogWrite(RLmotorEn, 0);
  }
}

void advance_forward3(float speedFR, float speedFL, float speedRR, float speedRL) { // 輸出前後都有
  // === FR馬達控制 ===
  if (speedFR > 0) {
    speedFR = map(speedFR, 0, 255, MIN_SPEED_FR, 255);
    digitalWrite(FRmotor1, LOW);
    digitalWrite(FRmotor2, HIGH);
  } else if (speedFR < 0) {
    speedFR = map(-speedFR, 0, 255, MIN_SPEED_FR, 255);
    digitalWrite(FRmotor1, HIGH);
    digitalWrite(FRmotor2, LOW);
  } else {
    speedFR = 0;
    digitalWrite(FRmotor1, LOW);
    digitalWrite(FRmotor2, LOW);
  }
  analogWrite(FRmotorEn, speedFR);

  // === FL馬達控制 ===
  if (speedFL > 0) {
    speedFL = map(speedFL, 0, 255, MIN_SPEED_FL, 255);
    digitalWrite(FLmotor1, LOW);
    digitalWrite(FLmotor2, HIGH);
  } else if (speedFL < 0) {
    speedFL = map(-speedFL, 0, 255, MIN_SPEED_FL, 255);
    digitalWrite(FLmotor1, HIGH);
    digitalWrite(FLmotor2, LOW);
  } else {
    speedFL = 0;
    digitalWrite(FLmotor1, LOW);
    digitalWrite(FLmotor2, LOW);
  }
  analogWrite(FLmotorEn, speedFL);

  // === RR馬達控制 ===
  if (speedRR > 0) {
    speedRR = map(speedRR, 0, 255, MIN_SPEED_RR, 255);
    digitalWrite(RRmotor1, LOW);
    digitalWrite(RRmotor2, HIGH);
  } else if (speedRR < 0) {
    speedRR = map(-speedRR, 0, 255, MIN_SPEED_RR, 255);
    digitalWrite(RRmotor1, HIGH);
    digitalWrite(RRmotor2, LOW);
  } else {
    speedRR = 0;
    digitalWrite(RRmotor1, LOW);
    digitalWrite(RRmotor2, LOW);
  }
  analogWrite(RRmotorEn, speedRR);

  // === RL馬達控制 ===
  if (speedRL > 0) {
    speedRL = map(speedRL, 0, 255, MIN_SPEED_RL, 255);
    digitalWrite(RLmotor1, LOW);
    digitalWrite(RLmotor2, HIGH);
  } else if (speedRL < 0) {
    speedRL = map(-speedRL, 0, 255, MIN_SPEED_RL, 255);
    digitalWrite(RLmotor1, HIGH);
    digitalWrite(RLmotor2, LOW);
  } else {
    speedRL = 0;
    digitalWrite(RLmotor1, LOW);
    digitalWrite(RLmotor2, LOW);
  }
  analogWrite(RLmotorEn, speedRL);
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

bool angle_detect(int angle) {
  if (pipe_exist) {
    return false;
  }
  checkGyroData(); // 取得仰角
  if ((millis() - gyro_lastTime) > 0) {
    gyro_lastTime = millis();
    if (pitch < -angle) { // 上坡
      if (pwm_output > 0) { // 左轉
        advance_forward1(255, 100, 255, 100);
      }
      else if (pwm_output < 0) { // 右轉
        advance_forward1(100, 255, 100, 255);
      }
      else {
        advance_forward1(220, 220, 220, 220);
      }
      delay(100);
      return true;
    }
    else if (pitch > 5) { // 下坡
      brake_light();
      if (pwm_output > 25) { // 左轉
        advance_forward2(0, -0.5, 0, 0);
      }
      else if (pwm_output < -25) { // 右轉
        advance_forward2(-0.5, 0, 0, 0);
      }
      else {
        advance_forward2(-0.5, -0.5, 0, 0);
      }
      // delay(100);
      return true;
    }
    else { // 平面
      return false;
    }
  }
  return false;
}

void resetGyro() { // 重置陀螺儀
  Serial.println("RESET");
  Serial2.write(0xFF);
  Serial2.write(0xAA);
  Serial2.write(0x52);
}

void checkGyroData() {
  readGyroData();
  if (dataReady) {
    dataReady = 0;
    if (dataBuffer[0] == 0x55 && dataBuffer[1] == 0x53) { // 檢查幀頭
      pitch = (short)((dataBuffer[3] << 8) | dataBuffer[2]) / 32768.0 * 180; // 計算俯仰角
    }
  }
}

void readGyroData() {
  while (Serial2.available()) {
    dataBuffer[dataCounter] = (unsigned char)Serial2.read();
    if (dataCounter == 0 && dataBuffer[0] != 0x55) return; // 如果第0字節不是幀頭，返回
    dataCounter++;

    if (dataCounter == 11) { // 接收到11個字節數據
      dataCounter = 0; // 重置計數器，準備接收下一幀數據
      dataReady = 1; // 標誌數據包已完全接收
      if (dataBuffer[0] != 0x55 || dataBuffer[1] != 0x53) dataReady = 0; // 檢查幀頭是否正確
    }
  }
}

void norm_red() {
  strip.clear();
  if (light_status == 2) {
    strip.fill(strip.Color(norm_light_level, 0, 0), 0, NUM_LEDS);
    strip.show();
  } else if (light_status != 0) {
    for (int i = 0; i <= norm_light_level; i++) {
      strip.fill(strip.Color(i, 0, 0), 0, NUM_LEDS);
      strip.show();
      delay(5);
    }
  } else {
    strip.fill(strip.Color(norm_light_level, 0, 0), 0, NUM_LEDS);
    strip.show();
  }
  light_status = 0;
}

void brake_light() {
  light_status = 2;
  strip.clear();
  strip.fill(strip.Color(255, 0, 0), 0, NUM_LEDS);
  strip.show();
}

void turn_r() {
  light_status = 1;
  strip.clear();
  for (int j = 0; j < NUM_LEDS; j++) { 
    strip.setPixelColor(j, strip.Color(150, 70, 0)); // 橘黃色 (R=255,G=100,B=0)
    strip.show();
    delay(40);
  }
  strip.clear();
  strip.show();
  delay(20);
}

void turn_l() {
  light_status = 1;
  strip.clear();
  for (int j = NUM_LEDS - 1; j >= 0 ; j--) { 
    strip.setPixelColor(j, strip.Color(150, 70, 0)); // 橘黃色 (R=255,G=100,B=0)
    strip.show();
    delay(40);
  }
  strip.clear();
  strip.show();
  delay(20);
}

void turn_signal() {
  if (pwm_output > 30) { 
    turn_l(); 
  } else if (pwm_output < -30) {
    turn_r(); 
  } else { 
    norm_red(); 
  }
}
