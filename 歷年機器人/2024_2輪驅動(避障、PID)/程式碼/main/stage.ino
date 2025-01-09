/*以下為切換關卡之程式,此處則自由運用，若不需要則可以刪除*/

void S_A(){ //避障+PID
  working_A();
}

void S_B(){ //上坡pid+U迴轉(右)
  if (S_B_UP==0){
    pid();
    S_B_UP_count+=1;
    Serial.print(S_B_UP_count);
    if(S_B_UP_count==800){S_B_UP=1;}
  }
  else{
    d_f = ping_d();
    if (d_f == 0) { d_f = 200; } 
    Serial.print("前方距離");
    Serial.println(d_f);
  
    if (d_f < 40) { /*柵欄處*/
      count_S_B_turn += 1;
      if ((count_S_B_turn > 30) && (S_B_turn != 2)) {
        
        advance_stop();
        delay(500);
        advance_back(255,255);
        delay(250);

        while (ANG > -90) {
          checkMPU();
          advance_right(125, 62);
          count_S_B_turn=0;

          
          if (ANG <= -89) {
            advance_stop();
            delay(250);
            count_S_B_turn=0;
            S_B_turn += 1;
            REset();
            checkMPU();
            
            break;
          }
        }
      
      }
    } else {
      count_S_B_turn -= 1;
      if (count_S_B_turn < 0) { count_S_B_turn = 0; }
      // advance_forward(255,255);
      pid();
    }
  }
}

void S_C(){ //直線pid
  pid();
}

void S_D(){ //U迴轉(左)
  d_f = ping_d();
  if (d_f == 0) { d_f = 200; } 
  // Serial.print("前方距離");
  // Serial.println(d_f);
  if (d_f < 30) { /*柵欄處*/
    ++count_S_D_turn;
    if ((count_S_D_turn > 20) && (S_D_turn != 2)) {
      advance_stop();
      delay(1000);
      while (ANG < 90) {
        checkMPU();
        advance_left(62, 125);
        if (ANG >= 89) {
          advance_stop();
          // delay(1500);
          S_D_turn += 1;
          REset();
          break;
        }
      }
      
    }
  } else {
    --count_S_D_turn;
    if (count_S_D_turn < 0) { count_S_D_turn = 0; }
    // advance_forward(255,255);
    pid();
  }
}

void S_E(){ //直線pid
  pid();
}

void S_B2(){ //U迴轉(右)
  d_f = ping_d();
  if (d_f == 0) { d_f = 200; } 
  Serial.print("前方距離");
  Serial.println(d_f);
  
  if (d_f < 40) { /*柵欄處*/
    count_S_B_turn += 1;
    if ((count_S_B_turn > 30) && (S_B_turn != 2)) {
        
      advance_stop();
      delay(500);
      advance_back(255,255);
      delay(250);

      while (ANG > -90) {
        checkMPU();
        advance_right(125, 62);
        count_S_B_turn=0;

        if (ANG <= -89) {
          advance_stop();
          delay(250);
          count_S_B_turn=0;
          S_B_turn += 1;
          REset();
          checkMPU();
          break;
        }
      }
    }
  }else {
    count_S_B_turn -= 1;
    if (count_S_B_turn < 0) { count_S_B_turn = 0; }
    // advance_forward(255,255);
    pid();
  }
}



