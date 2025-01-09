/*此為PID控制應用於自走車直線校正行走之實作*/
/*若有需要調整，請搭配註解以及您所需要的校正邏輯去修改*/

void pid() {  
  checkMPU();
  error = ANG;        //左邊正右邊負 左邊負右邊正
  integral += error;  
  output = (kp * error) + (ki * integral) + (kd * (error - lasterror));  //pid計算，參數根據賽道情況調整 ，在main分頁可以修改變數

  Serial.print("當前角度:");
  Serial.print(ANG);
  Serial.print("PID:");
  Serial.println(output);
  advance_forward(255,255);
  //              -3.0     <         0         <     3.0
  // 共3個區間 大右偏，向左修         往前           大左偏，向右修
            
  while (output != 0){  /*開始出現偏差*/
    if(output<-3.0){
      advance_forward(225, 255);
      Serial.println("向左修正");
      lasterror = error;
      break;
    }
    else if(output>=-3.0 && output<=3.0){
      advance_forward(255, 255);
      Serial.println("前走");
      lasterror = error;
      break;
    }
    else if(output>3.0){
      advance_forward(255, 225);
      Serial.println("向右修正");
      lasterror = error;
      break;
    }
  }
}


