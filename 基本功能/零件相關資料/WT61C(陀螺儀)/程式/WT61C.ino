/*陀螺儀*/

unsigned char Re_buf[11], counter = 0;
unsigned char sign = 0;
float ANG;
float roll, pitch, yaw;


void setup() {
  Serial2.begin(115200);

  /*WT61C陀螺儀初始化(若陀螺儀尚未連接至Arduino勿使用，否則程式無法繼續往下走)*/
  while (true) { 
    REset();
    checkMPU();
    if (ANG == 0 && roll == 0 && pitch == 0) break;
  }
}
void loop(){
  checkMPU();
  Serial.print("當前偏航角(yaw)之角度:");

  Serial.print(yaw);
}
