const int sensorPin = A0; 
int sensorValue = 0; 

void setup() {
  Serial.begin(9600);  
  pinMode(sensorPin, INPUT); 
}

void loop() {
  sensorValue = digitalRead(sensorPin);  
  if(sensorValue != 0){
    Serial.println("TOUCH");          
  }else{
    Serial.println("NO SIGNAL");     
  }
    delay(500);
}
