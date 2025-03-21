#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  sensor.setTimeout(500);
  while (1) {
    if (!sensor.init()) {
      Serial.println("感測器初始化失敗！");
      delay(1000);
    } else {
      break;
    }
  }

  // 設置感測範圍模式（可選），選擇更長距離但精度較低的模式
  if (sensor.setMeasurementTimingBudget(80)) { // 設定測量計時預算為80ms
    Serial.println("成功設置測量計時預算為80ms");
  } else {
    Serial.println("設置測量計時預算失敗，請檢查預算值是否有效");
  }
}

void loop() {
  Serial.print("距離: ");
  Serial.print(sensor.readRangeSingleMillimeters() / 10); // 將毫米轉換為厘米
  Serial.println(" cm");

  if (sensor.timeoutOccurred()) {
    Serial.println("讀取超時！");
  }

  delay(100);
}
