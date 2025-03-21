unsigned char dataBuffer[11];
unsigned char dataCounter = 0;
unsigned char dataReady = 0;
float yaw = 0.0, pitch = 0.0, roll = 0.0; // 三軸角度

unsigned long timer = 0;
unsigned long lastPrintTime = 0;
float target = 0; // 設置目標角度
float error = 0;
float integral = 0;
float derivative = 0;
float lastError = 0;
float angle = 0;

// 定義L298N模塊控制馬達的引腳（前輪馬達）
const int ENA1 = 6;   // 馬達1速度控制
const int IN1 = 7;    // 馬達1轉向控制
const int IN2 = 8;    // 馬達1轉向控制
const int IN3 = 9;    // 馬達2轉向控制
const int IN4 = 10;   // 馬2轉向控制
const int ENB1 = 11;  // 馬達2速度控制

// 定義L298N模塊控制馬達的引腳（後輪馬達）
const int ENA2 = A0;  // 馬達3速度控制
const int IN5 = A1;   // 馬達3轉向控制
const int IN6 = A2;   // 馬達3轉向控制
const int IN7 = A3;   // 馬達4轉向控制
const int IN8 = A4;   // 馬達4轉向控制
const int ENB2 = A5;  // 馬達4速度控制

int motorSpeed = 200; // 馬達基礎速度

float Kp = 250; // 比例增益 
float Ki = 1; // 積分增益 
float Kd = 1; // 微分增益

void setup() {
  Serial.begin(115200); // 鮑率可調
  Serial1.begin(115200); // 與WT61C-TTL通訊的串口

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

  while (true) {
    resetGyro();
    checkGyroData();
    if (yaw == 0.0 && roll == 0.0 && pitch == 0.0) break;
  }
}

void loop() {
  checkGyroData(); // 更新陀螺儀數據
  if((millis() - timer) > 10){ // 每10ms執行一次    
    pid(); // 執行PID控制
    timer = millis();
  }
    if((millis() - lastPrintTime) > 500){ // 每500ms執行一次
    Serial.print("yaw(Z): "); // 水平角度，若單純想走直線PID用此角度即可
    Serial.println(yaw);
    lastPrintTime = millis();
  }
}

void resetGyro() { // 重置陀螺儀
  Serial1.write(0xFF);
  Serial1.write(0xAA);
  Serial1.write(0x52);
}

void checkGyroData() {
  readGyroData();
  if (dataReady) {
    dataReady = 0;
    if (dataBuffer[0] == 0x55 && dataBuffer[1] == 0x53) { // 檢查幀頭
      yaw = (short(dataBuffer[7] << 8 | dataBuffer[6])) / 32768.0 * 180; // 計算偏航角
      pitch = (short(dataBuffer[3] << 8 | dataBuffer[2])) / 32768.0 * 180; // 計算俯仰角
      roll = (short(dataBuffer[5] << 8 | dataBuffer[4])) / 32768.0 * 180; // 計算滾轉角
    }
  }
}

void readGyroData() {
  while (Serial1.available()) {
    dataBuffer[dataCounter] = (unsigned char)Serial1.read();
    if (dataCounter == 0 && dataBuffer[0] != 0x55) return; // 如果第0字節不是幀頭，返回
    dataCounter++;

    if (dataCounter == 11) { // 接收到11個字節數據
      dataCounter = 0; // 重置計數器，準備接收下一幀數據
      dataReady = 1; // 標誌數據包已完全接收
      if (dataBuffer[0] != 0x55 || dataBuffer[1] != 0x53) dataReady = 0; // 檢查幀頭是否正確
    }
  }
}

void pid() {
  error = target - yaw; // 獲取yaw角度的誤差
  integral += error; // 積分
  // integral = constrain(integral, -100, 100); // 限制積分範圍，避免積分飽和
  derivative = error - lastError; // 微分
  angle = (error * Kp) + (integral * Ki) + (derivative * Kd); // 計算PID輸出

  // 限制PID輸出的範圍
  angle = constrain(angle, -255, 255);

 if (error == 0) {
    advance_forward(motorSpeed, motorSpeed, motorSpeed, motorSpeed); // 保持直行
  } else {
    advance_forward(motorSpeed + angle, motorSpeed - angle, motorSpeed + angle, motorSpeed - angle); // 調整輪速(左+右-)
  }
    lastError = error; // 更新上一次誤差
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
