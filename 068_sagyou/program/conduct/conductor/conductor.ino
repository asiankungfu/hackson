// C++ code
//
const int LED_PIN = 7;
const int BTN_PINS[] = {4, 3, 2, 1};
const int NUM_BTNS = 4;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  for(int i = 0; i < NUM_BTNS; i++){
  	pinMode(BTN_PINS[i], INPUT_PULLUP);
  }
  
  for(int i = 0; i < 3; i++){
  	digitalWrite(LED_PIN, HIGH); delay(40);
  	digitalWrite(LED_PIN, LOW);  delay(40);
  }
}

void loop()
{
  for(int i = 0; i < NUM_BTNS; i++){
    if(digitalRead(BTN_PINS[i]) == LOW){
    
      blinkLED(i + 1);
    
      while(digitalRead(BTN_PINS[i]) == LOW);
      delay(10);
    }
  }
}


void blinkLED(int count){
  for(int j = 0; j < count; j++){
  digitalWrite(LED_PIN, HIGH);
  delay(250);
  digitalWrite(LED_PIN, LOW);
  
    if(j < count -1){
    delay(500);
    }
  }
}