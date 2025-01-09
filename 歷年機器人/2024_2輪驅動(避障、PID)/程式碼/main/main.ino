/*此為一2輪驅動外加1萬向輪之機器人，具避障、PID直線校正、關卡切換功能*/

#include <Servo.h>
#include <Keypad.h> 

/*陀螺儀&PID*/
/*此處關於pid的參數可依照測試去修改*/
float kp = 2.0;
float ki = 0.0;
float kd = 0.0;

unsigned char Re_buf[11], counter = 0;
unsigned char sign = 0;
float ANG;
float roll, pitch, yaw;
float output = 0.0;
float error = 0.0;
float lasterror = 0.0;
float integral = 0.0;

/*馬達*/
//基本上此處則依照L298N上腳位進行修改
/*Motor_I可以插digital*/
#define MotorA_I1 40
#define MotorA_I2 42 /*左前輪*/
#define MotorB_I1 41
#define MotorB_I2 43 /*右前輪*/

/*以下腳位請插PWM*/
#define MotorA_PWMA 6
#define MotorB_PWMB 7

/*伺服馬達*/
/*以下腳位請插PWM*/
#define d_servopin 10
Servo d_servo;

/*超音波*/
/*以下腳位請插PWM*/
int F_Echo = 11;
int F_Trig = 12;

/*以下為切換關卡使用到之變數，此處則自由運用*/
int count_hr=0;
int count_lr=0;
int S_B_UP_count=0;
int count_S_B_turn=0;
int S_B_UP=0;
int S_B_turn=0;
int count_S_D_turn=0;
int S_D_turn=0;
float d_f=0.0;
float d_l=0.0;
float d_r=0.0;
char stage;

/*鍵盤按鍵(切換關卡)*/
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {      //定義 4*4 keypad 矩陣
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = { 28, 26, 24,22 };         //4*4 keypad 連接 Arduino 方式
byte colPins[COLS] = {36, 34, 32, 30  };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  /*此處則進行元件初始化設置，若有調整需求請至pinset分頁修改*/
  pinset();
}

void loop() {
  char key =keypad.getKey();
  if (stage==NULL){ // 取得欲切換關卡輸入,選擇完關卡後進到該關卡之程式
    stage=key;
    Serial.println("choose stage");
  }

  if (stage == 'A')       //以stage變數去切換關卡
  {
    S_A();
  }
  else if(stage == 'B'){
    S_B(); 
  }
  else if(stage == 'C'){
    S_C();
  }
  else if(stage=='D'){
    S_D();
  }
  else if(stage=='E'){
    S_E();
  }
  else if(stage=='1'){ //直線PID功能
    pid();
  }
  else if(stage=='2'){  //僅進行U迴轉(右)
    S_B2();
  }
}
