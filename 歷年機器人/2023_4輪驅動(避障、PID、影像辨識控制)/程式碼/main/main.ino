/*此為4輪驅動之機器人，具避障、PID直線校正、進行影像辨識控制之功能*/
/*影像辨識主要由樹莓派去處理，若辨識到目標之後則會通訊給Arduino，以進行辨識任務(夾取、噴灑)*/

#include <Servo.h>

/*陀螺儀&PID*/
unsigned char Re_buf[11], counter = 0;
unsigned char sign = 0;
float ANG;
float roll, pitch, yaw;
float kp = 10;
float ki = 0;
float kd = 200;
float output = 0.0;  //輸出
float error = 0.0;
float lasterror = 0.0;
float integral = 0.0;

/*馬達*/
#define MotorA_I1 40
#define MotorA_I2 42 /*左前輪*/
#define MotorB_I1 41
#define MotorB_I2 43 /*右前輪*/
#define MotorC_I1 44
#define MotorC_I2 46 /*左後輪*/
#define MotorD_I1 45
#define MotorD_I2 47 /*右後輪*/
#define MotorA_PWMA 6
#define MotorB_PWMB 7
#define MotorC_PWMC 4
#define MotorD_PWMD 5 /*馬達驅動*/
/*伺服馬達&繼電器*/
#define servo1_pin 2  /*夾爪伺服馬達*/
#define servo2_pin 11 /*手臂伺服馬達*/
#define relaypin 8    /*繼電器*/
#define d_servopin 10 /*超音波下的伺服馬達*/

Servo d_servo;
Servo servo1;
Servo servo2;
/*超音波*/
int F_Echo = 12;
int F_Trig = 13;
int count_hr;
float d_f;
float d_l;
float d_r;
char incomedata;
int count_ox = 0;
int Mushroomtype = 0;
int grabb = 0;
int D_turn = 0;
int FGH = 0;

void setup() {
  pinset();
}

void loop() {
  
  pid1();
  }
