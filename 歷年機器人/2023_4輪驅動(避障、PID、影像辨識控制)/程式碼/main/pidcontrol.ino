void pid1() { /*給正常賽道的pid(較快)*/
  checkMPU();

  error = ANG;        //左邊正右邊負 左邊負右邊正
  integral += error;  
  output = (kp * error) + (ki * integral) + (kd * (error - lasterror)); //pid計算 參數根據賽道情況調整 
  Serial.print("當前角度:");
  Serial.print(ANG);
  Serial.print("PID:");
  Serial.println(output);
  advance_forward(255, 255, 255, 255);
  while (output != 0) /*開始出現偏差*/
  {
    if (output < 0) /* 代表往右偏了 */
    {
      if (output > -3) {
        advance_forward(255, 255, 255, 255);
        Serial.print("前走");
        lasterror = error;
        break;
      } else if (output < -3) {
        advance_forward(125, 255, 125, 255);
        Serial.print("向左修正");
        lasterror = error;
        break;
      }
    } else if (output > 0) /*代表往左偏了*/
    {
      if (output < 3) {
        advance_forward(255, 255, 255, 255);
        Serial.print("前走");
        lasterror = error;
        break;
      } else if (output > 3) {
        advance_forward(255, 125, 255, 125);
        Serial.print("向右修正");
        lasterror = error;
        break;
      }
    }
  }
}
void pid2() { /*給辨識賽道的pid(較慢)*/
  checkMPU();
  error = ANG;        //左邊正右邊負
  integral += error;  //左邊負右邊正
  output = (kp * error) + (ki * integral) + (kd * (error - lasterror));
  Serial.print("當前角度:");
  Serial.print(ANG);
  Serial.print("PID:");
  Serial.println(output);
  advance_forward(100, 100, 100,100);
  while (output != 0) /*開始出現偏差*/
  {
    if (output < 0) /* 代表往右偏了 */
    {
      if (output > -2) {
        advance_forward(100, 100, 100, 100);
        Serial.print("前走");
        lasterror = error;
        break;
      } else if (output < -2) {
        advance_forward(100, 110, 100, 110);
        Serial.print("向左修正");
        lasterror = error;
        break;
      }
    } else if (output > 0) /*代表往左偏了*/
    {
      if (output < 2) {
        advance_forward(100,100,100,100);
        Serial.print("前走");
        lasterror = error;
        break;
      } else if (output > 2) {
        advance_forward(110,100,110,100);
        Serial.print("向右修正");
        lasterror = error;
        break;
      }
    }
  }
}
