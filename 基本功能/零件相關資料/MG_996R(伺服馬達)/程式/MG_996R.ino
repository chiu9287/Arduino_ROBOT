#include <Servo.h>

// 定義伺服馬達接腳
#define SERVO_PIN 9

// 建立Servo物件
Servo servoMotor;

void setup() {
  // 初始化串口通訊
  Serial.begin(9600);
  
  // 連接伺服馬達到指定的腳位
  servoMotor.attach(SERVO_PIN);
  
  // 伺服馬達歸位
  servoMotor.write(90);
  delay(500); // 等待伺服馬達就位
}

void loop() {
  // 從串口接收指令
  if (Serial.available() > 0) {
    int angle = Serial.parseInt(); // 讀取角度值
    
    // 確保角度在有效範圍內
    if (angle >= 0 && angle <= 180) {
      // 控制伺服馬達轉動到指定角度
      servoMotor.write(angle);
      Serial.print("Moving to angle: ");
      Serial.println(angle);
    } else {
      Serial.println("Invalid angle! Angle should be between 0 and 180 degrees.");
    }
  }
}
