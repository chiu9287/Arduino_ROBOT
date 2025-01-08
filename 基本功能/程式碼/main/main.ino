/*SR04超音波(探測距離)*/
/*以下腳位請插PWM*/
int F_Echo = 11;
int F_Trig = 12;

/*L298N(馬達控制)*/
//基本上此處則依照L298N上腳位進行修改
/*Motor_I可以插digital*/
#define MotorA_I1 40
#define MotorA_I2 42 /*左前輪*/
#define MotorB_I1 41
#define MotorB_I2 43 /*右前輪*/

/*以下腳位請插PWM*/
#define MotorA_PWMA 6
#define MotorB_PWMB 7

/*WT61C陀螺儀&PID(直線校正)*/
/*此處關於pid的參數可依照測試去修改*/
// float kp = 2.0;
// float ki = 0.0;
// float kd = 0.0;

// unsigned char Re_buf[11], counter = 0;
// unsigned char sign = 0;
// float ANG;
// float roll, pitch, yaw;
// float output = 0.0;
// float error = 0.0;
// float lasterror = 0.0;
// float integral = 0.0;

void setup() {
  /*此處則進行元件初始化設置，若有調整需求請至pinset分頁修改*/
  pinset();
}
void loop(){
  // ping_d();//測試超音波
  // advance_forward(255,255) //測試馬達轉動

}
