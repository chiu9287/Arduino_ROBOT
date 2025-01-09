void communicate() {
  while (Serial.available() >= 0)  // 串口接收到数据
  {
    incomedata = Serial.read();  // 获取串口接收到的数据

    /* ////(1紅菇2藍菇3黃菇4圈5差6指定紅/藍/黃 9噴完水 10夾完摩菇)////////////////////////////////////////////////////////////////// */
    if (incomedata == '6') { /*接收到指定蘑菇顏色*/
      Mushroomtype = 1;
      advance_forward(80, 80, 80, 80);
      servo1.write(100);
    } else if (incomedata == '7') {
      Mushroomtype = 2;
      advance_forward(80, 80, 80, 80);
      servo1.write(100);
    } else if (incomedata == '8') {
      Mushroomtype = 3;
      advance_forward(80, 80, 80, 80);
      servo1.write(100);
    }

    /* ///////////////////////////////////////////////////////////////////////////////////////////// */
    else if (incomedata == '1' && Mushroomtype == 1&&grabb==0) { /*指定紅色且辨識到紅色蘑菇*/
      advance_stop();
      grab();
      delay(1000);
      advance_forward(255, 255, 255, 255);
      delay(10000);
      drop();
      advance_stop();
      servo1.write(10);
      servo2.write(0);
      incomedata = '10';
    } else if (incomedata == '2' && Mushroomtype == 2&&grab==0) { /*指定藍色且辨識到藍色蘑菇*/
      advance_stop();
      grab();
      delay(1000);
      advance_forward(255, 255, 255, 255);
      delay(10000);
      advance_stop();
      drop();
      servo1.write(10);
      servo2.write(0);
      incomedata = '10';
    } else if (incomedata == '3' && Mushroomtype == 3&&grab==0) { /*指定為黃色且辨識到黃色蘑菇*/
      advance_stop();
      grab();
      delay(1000);
      advance_forward(255, 255, 255, 255);
      delay(10000);
      advance_stop();
      drop();
      servo1.write(10);
      servo2.write(0);
      incomedata = '10';
    }
    /* ///////////////////////////////////////////////////////////////////////////////////////////// */
    else if (incomedata == '4' && count_ox < 3) { /*偵測到圈(右邊)*/
      ++count_ox;
      advance_forward(125, 125, 125, 125);
      shoot();
      delay(3000);
      stopshoot();
    } else if (incomedata == '4' && count_ox >= 3) { /*偵測到圈(左邊)*/
      ++count_ox;
      advance_forward(110, 110, 110, 110);
      shoot();
      delay(3000);
      stopshoot();
    } else if (incomedata == '5') { /*偵測到叉*/
      ++count_ox;
      /*//////////////////////////////////////////////////////////////////////////////////////////////////*/
    } else if (count_ox == 3) {
      advance_stop();
      delay(500);
      advance_forward(125, 125, 125, 125);
      delay(5000);
      while (ANG < 180) {
        advance_left(255, 255, 255, 255);
        checkMPU();
        Serial.println(ANG);
        if (ANG > 179) {
          advance_stop();
          REset();
          break;
        }
      }
    } else if (count_ox == 6) {
      incomedata == '9';
    } else {
      Serial.println("辨識的pid");
      //advance_forward(125, 125, 125, 125);
      pid2();
      //working2();
    }
    break;
  }
}
void communicate2() {
  while (Serial.available() >= 0)  // 串口接收到数据
  {
    incomedata = Serial.read();  // 获取串口接收到的数据

    /* ////(1紅菇2藍菇3黃菇4圈5差6指定紅/藍/黃 9噴完水 10夾完摩菇)////////////////////////////////////////////////////////////////// */
    if (incomedata == '6') { /*接收到指定蘑菇顏色*/
      Mushroomtype = 1;
      advance_forward(80, 80, 80, 80);
      servo1.write(100);
    } else if (incomedata == '7') {
      Mushroomtype = 2;
      advance_forward(80, 80, 80, 80);
      servo1.write(100);
    } else if (incomedata == '8') {
      Mushroomtype = 3;
      advance_forward(80, 80, 80, 80);
      servo1.write(100);
    }

    /* ///////////////////////////////////////////////////////////////////////////////////////////// */
    else if (incomedata == '1' && Mushroomtype == 1 && grabb == 0) { /*指定紅色且辨識到紅色蘑菇*/
      advance_stop();
      
      grab();
      delay(1000);
      advance_forward(255, 255, 255, 255);
      delay(10000);
      advance_stop();
      drop();
      delay(500);
      advance_forward(255,255,255,255);
      delay(5000);
      servo1.write(10);
      servo2.write(0);
      incomedata = '10';
    } else if (incomedata == '2' && Mushroomtype == 2 && grabb == 0) { /*指定藍色且辨識到藍色蘑菇*/
      advance_stop();
      grab();
      delay(1000);
      advance_forward(255, 255, 255, 255);
      delay(10000);
      advance_stop();
      drop();
      delay(500);
      advance_forward(255,255,255,255);
      delay(5000);
      servo1.write(10);
      servo2.write(0);
      incomedata = '10';
    } else if (incomedata == '3' && Mushroomtype == 3 && grabb == 0) { /*指定為黃色且辨識到黃色蘑菇*/
      advance_stop();
      grab();
      delay(1000);
      advance_forward(255, 255, 255, 255);
      delay(10000);
      advance_stop();
      drop();
      delay(500);
      advance_forward(255,255,255,255);
      delay(5000);
      servo1.write(10);
      servo2.write(0);
      incomedata = '10';
    }
    /* ///////////////////////////////////////////////////////////////////////////////////////////// */
    else if (incomedata == '4' && count_ox < 3) { /*偵測到圈(右邊)*/
      ++count_ox;
      advance_forward(125, 125, 125, 125);
      shoot();
      delay(3000);
      stopshoot();
    } else if (incomedata == '4' && count_ox >= 3) { /*偵測到圈(左邊)*/
      ++count_ox;
      advance_forward(110, 110, 110, 110);
      shoot();
      delay(3000);
      stopshoot();
    } else if (incomedata == '5') { /*偵測到叉*/
      ++count_ox;
      /*//////////////////////////////////////////////////////////////////////////////////////////////////*/
    } else if (count_ox == 3) {
      advance_stop();
      delay(500);
      advance_forward(125, 125, 125, 125);
      delay(5000);
      while (ANG < 180) {
        advance_left(255, 255, 255, 255);
        checkMPU();
        Serial.println(ANG);
        if (ANG > 179) {
          advance_stop();
          REset();
          break;
        }
      }
    } else if (count_ox == 6) {
      incomedata == '9';
    } else {
      Serial.println("辨識的pid");
      //advance_forward(125, 125, 125, 125);
      advance_forward(90, 90, 90, 90);
      //working2();
    }
    break;
  }
}