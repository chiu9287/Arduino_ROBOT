#include "Wire.h" //必要函式庫
#include <MPU6050_light.h>

MPU6050 mpu(Wire);//設置MPU
unsigned long timer = 0;

void setup() { //設置並初始化陀螺儀
  Serial.begin(9600); //鮑率可調
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print("MPU6050 狀態: ");
  Serial.println(status);
  while(status!=0){ } //若未連結到MPU6050則執行{}處，即不動作
  
  Serial.println("勿觸碰陀螺儀，計算中");
  delay(2000);
  // mpu.upsideDownMounting = true; // 若陀螺儀為翻轉狀態放車上或附著於車底下則取消註解此行
  mpu.calcOffsets(); // 計算
}

void loop() { //持續更新陀螺儀資料並印出
  mpu.update();
  
  if((millis()-timer)>10){ //每10ms印出資料

	// Serial.print("  pitch(X) : "); //若有特殊角度需求可使用
	// Serial.print(mpu.getAngleX());

	// Serial.print("  roll(Y) : ");
	// Serial.print(mpu.getAngleY());

	Serial.print("  yaw(Z) : "); //水平角度，若單純想走直線PID用此角度即可
	Serial.println(mpu.getAngleZ());
	timer = millis();  
  }
}
