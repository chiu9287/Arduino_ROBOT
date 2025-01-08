/*馬達*/
#define MotorA_I1 42
#define MotorA_I2 40 /*左前輪*/
#define MotorB_I1 43
#define MotorB_I2 41 /*右前輪*/

#define MotorA_PWMA 6
#define MotorB_PWMB 7


void setup() {
  Serial.begin(115200);

  pinMode(MotorA_I1, OUTPUT); /*馬達設置*/
  pinMode(MotorA_I2, OUTPUT);
  pinMode(MotorB_I1, OUTPUT);
  pinMode(MotorB_I2, OUTPUT);
  pinMode(MotorA_PWMA, OUTPUT);
  pinMode(MotorB_PWMB, OUTPUT);

}

void loop() {

}
