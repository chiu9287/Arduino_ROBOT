/*此處為避障+pid邏輯結合*/
void working_A(){
  d_f = ping_d();
  if (d_f == 0) { d_f = 200; } 
  // Serial.print("前方距離");
  // Serial.println(d_f);
  if (d_f < 35) { /*當正前方距離太近，停下，測試左右距離並記錄下來*/
    ++count_hr;
    // Serial.println("濾波++");
    if (count_hr>10) {
      count_hr=0;
      advance_stop();
      delay(500);
      Serial.println("停止");

      //以下為轉頭邏輯//
      d_servo.write(45); /*右前*/
      delay(500);
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

      /*開始判斷左右距離*/
      //判斷完會轉彎後往前一小段距離，先繞過障礙物後，回到這段程式的最上面，
      if (d_l > d_r) {
        advance_left(255, 255); //轉彎幅度
        delay(550);

        advance_forward(255, 255); //轉彎後往前的距離
        delay(250);
        Serial.println("往左轉");

      } else if (d_r > d_l) {
        advance_right(255, 255);
        delay(550);
        advance_forward(255, 255);
        delay(250);
        Serial.println("往右轉");

      }else{
        ++count_lr;
        if(count_lr==2){ //鬼打牆，往後退
          advance_back(255,255);
          count_lr=0;
        }
      }

    }
  } else {  //前方沒有偵測到東西，往前走或pid
    --count_hr;
    if (count_hr < 0) { count_hr = 0; }
    pid();
    // advance_forward(255,255);
  }
}
