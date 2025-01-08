void pinset() {
  Serial.begin(115200); 
  // Serial2.begin(115200); //WT61C陀螺儀

  pinMode(MotorA_I1, OUTPUT); /*馬達設置*/
  pinMode(MotorA_I2, OUTPUT);
  pinMode(MotorB_I1, OUTPUT);
  pinMode(MotorB_I2, OUTPUT);
  pinMode(MotorA_PWMA, OUTPUT);
  pinMode(MotorB_PWMB, OUTPUT);


  pinMode(F_Trig, OUTPUT); /*超音波設置*/
  pinMode(F_Echo, INPUT);


  // while (true) { /*WT61C陀螺儀初始化(若陀螺儀尚未連接至Arduino勿使用，否則程式無法繼續往下走)*/
  //   REset();
  //   checkMPU();
  //   if (ANG == 0 && roll == 0 && pitch == 0) break;
  // }

}