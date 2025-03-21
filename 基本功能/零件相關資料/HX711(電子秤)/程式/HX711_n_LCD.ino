#include "HX711.h"
#include <Wire.h> // I2C程式庫
#include <LiquidCrystal_I2C.h> // LCD_I2C模組程式庫

// LCD I2C位址，默認為0x27或0x3F，依據背板的晶片不同而有差異，16、2為LCD顯示器大小。
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// HX711 接線設定
const int DT_PIN = 13;
const int SCK_PIN = 12;
const float scale_factor = -698.5;

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(scale_factor);  // 開始取得比例參數
  scale.tare();

  // 初始化LCD
  lcd.init();
  lcd.backlight();

  Serial.println("Nothing on it!.");
  lcd.setCursor(0, 0); // (colum, row)從第一排的第一個位置開始顯示
  lcd.print("Nothing on it!");
}

void loop() {
  float current_weight = scale.get_units(5);  // 取得5次數值的平均
  if (abs(current_weight) < 0.4) {
    current_weight = 0.0;
  }else if (current_weight > 60) { // 重鵝
    current_weight = 62;           // 丟藍色
  }else if (current_weight > 45) { // 中鵝
    current_weight = 50;           // 丟綠色
  }else if (current_weight > 25) { // 輕鵝
    current_weight = 33;           // 丟紅色
  }

  Serial.print("Weight :");
  Serial.println(current_weight,2);

  lcd.clear(); // 清除上次顯示的內容
  lcd.setCursor(0, 0); // (colum, row)從第一排的第一個位置開始顯示
  lcd.print("Weight :"); 
  lcd.setCursor(0, 1); // (colum,row)從第二排第一格位置開始顯示
  lcd.print(current_weight,0);
  lcd.print(" g");
  
}
