#include <Wire.h> // I2C程式庫
#include <LiquidCrystal_I2C.h> // LCD_I2C模組程式庫

// LCD I2C位址，默認為0x27或0x3F，依據背板的晶片不同而有差異，16、2為LCD顯示器大小。
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {  
  // 初始化LCD
  lcd.init();
  lcd.backlight();
}

void loop() {
  lcd.clear(); // 清除上次顯示的內容
  // 在LCD上顯示Hello World!
  lcd.setCursor(2, 0); // (colum, row)從第一排的第三個位置開始顯示
  lcd.print("Hello World!"); 
  lcd.setCursor(4, 1); // (colum,row)從第二排第三格位置開始顯示
  lcd.print("test123");
  delay(100);
}
