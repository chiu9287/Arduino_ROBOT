int F_Echo = 11;
int F_Trig = 12;
float duration,distance;

void setup() {
  Serial.begin(115200);

  pinMode(F_Trig, OUTPUT); /*超音波腳位設置*/
  pinMode(F_Echo, INPUT);
}
void loop(){
  ping_d();
}

