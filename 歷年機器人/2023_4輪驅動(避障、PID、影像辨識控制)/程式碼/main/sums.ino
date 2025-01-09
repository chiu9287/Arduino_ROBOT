void working1() { /*持續跑，偏了透過pid回正；當測到距離小於40選擇左右轉*/
  d_f = ping_d();
  Serial.println(d_f);
  if (d_f < 50 && d_f > 5) { /*當正前方距離太近，停下，測試左右距離並記錄下來*/
    ++count_hr;  //濾波計數器++
    if (count_hr > 5) {   //(確定前方真有障礙物)
      advance_stop();
      Serial.println("停止");
      d_servo.write(45); /*右前*/
      delay(1000);
      d_r = ping_d(); /*紀錄右前距離*/
      if (d_r == 0) { d_r = 200; }
      Serial.println("右邊距離為");
      Serial.println(d_r);
      d_servo.write(135); /*左前*/
      delay(1000);
      d_l = ping_d(); /*紀錄左前距離*/
      if (d_l == 0) { d_l = 200; }
      Serial.println("左前距離");
      Serial.println(d_l);
      d_servo.write(90);
      delay(500);
      count_hr = 0;

      /*開始判斷左右距離*/
      if (d_l > d_r) {
        advance_left(255, 255, 255, 255);
        delay(900);
        advance_forward(255, 255, 255, 255);
        delay(1000);
        Serial.println("往左轉");
        d_l = 0;
        d_r = 0;
        d_f = 0;
      } else if (d_r > d_l) {
        advance_right(255, 255, 255, 255);
        delay(900);
        advance_forward(255, 255, 255, 255);
        delay(100);
        Serial.println("往右轉");
        d_l = 0;
        d_r = 0;
        d_f = 0;
      }
    }
  } else {
    --count_hr; //濾波計數器--
    if (count_hr < 0) { count_hr = 0; }
    pid1();
  }
}
void working3() { /*FGH*/
  d_servo.write(165);
  d_f = ping_d();
  Serial.println(d_f);
  if (d_f > 50) { /*左前方照到無柵欄處*/
    ++count_hr;
    if ((count_hr == 120) && (FGH == 0)) {
      Serial.print("進到無柵欄");
      advance_forward(255, 255, 255, 255);
      delay(1000);
      while (ANG < 90) {
        advance_left(255, 255, 255, 255);
        checkMPU();
        if (ANG > 89) {
          advance_stop();
          REset();
          break;
        }
      }
      FGH = 1;
    }
  } else {
    //--count_hr;
    if (count_hr < 0) { count_hr = 0; }
    pid1();
  }
}

void working2() { /*持續跑，偏了透過pid回正；當測到距離小於40選擇左右轉*/
  d_f = ping_d();
  Serial.println(d_f);
  if (d_f < 50 && d_f > 5) { /*當正前方距離太近，停下，測試左右距離並記錄下來*/
    ++count_hr;
    if (count_hr > 5) {
      advance_stop();
      Serial.println("停止");
      d_servo.write(45); /*右前*/
      delay(1000);
      d_r = ping_d(); /*紀錄右前距離*/
      if (d_r == 0) { d_r = 200; }
      Serial.println("右邊距離為");
      Serial.println(d_r);
      d_servo.write(135); /*左前*/
      delay(1000);
      d_l = ping_d(); /*紀錄左前距離*/
      if (d_l == 0) { d_l = 200; }
      Serial.println("左前距離");
      Serial.println(d_l);
      d_servo.write(90);
      delay(500);
      count_hr = 0;

      /*開始判斷左右距離*/
      if (d_l > d_r) {
        advance_left(255, 255, 255, 255);
        delay(900);
        advance_forward(255, 255, 255, 255);
        delay(600);
        Serial.println("往左轉");
        d_l = 0;
        d_r = 0;
        d_f = 0;
      } else if (d_r > d_l) {
        advance_right(255, 255, 255, 255);
        delay(900);
        advance_forward(255, 255, 255, 255);
        delay(600);
        Serial.println("往右轉");
        d_l = 0;
        d_r = 0;
        d_f = 0;
      }
    }
  } else {
    --count_hr;
    if (count_hr < 0) { count_hr = 0; }
    pid2();
  }
}

void S_ABC() {
  working1();
}
void S_D() {
  if (count_ox != 3) {
    communicate();

    Serial.print("還在噴前");
  } else if (count_ox == 3 && D_turn == 0) {
    advance_forward(255, 255, 255, 255);
    delay(5000);
    ANG=0;
    while (ANG < 90) {
      advance_left(255, 255, 255, 255);
      checkMPU();
      if (ANG > 89) {
        advance_stop();
        REset();
        break;
      }
    }
    advance_forward(255,255,255,255);
    delay(2000);
    D_turn = 1;
  } 
    /*advance_forward(255,255,255,255);
    delay(5000);
    advance_left(255,255,255,255);
    delay(1000);
    advance_forward(255,255,255,255);
    delay(3000);
    */
  }
void S_E() {
  if (grab != 1) {
    communicate2();
  } else if (grabb == 1) {
    advance_forward(255, 255, 255, 255);
  }
}

void S_FH(){
  pid1();
}

void S_G() {
  working3();
}

