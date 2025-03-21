unsigned char dataBuffer[11];
unsigned char dataCounter = 0;
unsigned char dataReady = 0;
float roll = 0.0, pitch = 0.0, yaw = 0.0; // 三軸角度
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  while (true) {
    resetGyro();
    checkGyroData();
    if (yaw == 0.0 && roll == 0.0 && pitch == 0.0) break;
  }
}

void loop() {
  checkGyroData();

  if ((millis() - lastPrintTime) > 100) { // 每100ms印出資料
    Serial.print("Yaw: "); Serial.println(yaw);
    //Serial.print("Roll: "); Serial.println(roll);
    //Serial.print("Pitch: "); Serial.println(pitch);
    lastPrintTime = millis();
  }
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
      yaw = (short(dataBuffer[7] << 8 | dataBuffer[6])) / 32768.0 * 180; // 計算偏航角
      pitch = (short(dataBuffer[3] << 8 | dataBuffer[2])) / 32768.0 * 180; // 計算俯仰角
      roll = (short(dataBuffer[5] << 8 | dataBuffer[4])) / 32768.0 * 180; // 計算滾轉角
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
