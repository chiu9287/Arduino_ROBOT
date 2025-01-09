void grab() {
  for (int i = 0; i <= 102; i += 1) { /*手臂向下*/
    servo2.write(i);
    delay(20);
  }
  for (int i = 100; i >= 13; i -= 1) { /*夾*/
    servo1.write(i);
    delay(20);
  }
  delay(1000);

  servo2.write(30);
  delay(1000);
}
void drop() {
  for (int i = 30; i <= 102; i += 1) { /*手臂向下*/
    servo2.write(i);
    delay(20);
  }
  for (int i = 13; i <= 80; i += 1) { /*放*/
    servo1.write(i);                   // 使用write，傳入角度，從0度轉到180度
    delay(20);
  }
}
