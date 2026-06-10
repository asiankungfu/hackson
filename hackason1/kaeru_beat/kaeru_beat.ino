const int sensorPin = A0; 
const int ledPin = 13;    
const int threshold = 600;    // テストで確定したしきい値
const int targetCount = 13;    // 担当楽器の起動に必要な点滅回数

boolean isLightOn = false;
int currentCount = 0;

void setup() {
  // 設計書通りの高速通信
  Serial.begin(115200);   
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // 【点滅の検知】暗状態から明状態への変化を捉える
  if (sensorValue > threshold && isLightOn == false) {
    isLightOn = true;
    currentCount++;
    digitalWrite(ledPin, HIGH); // 検知した瞬間に基板LEDを点灯

    delay(50);
    
    // 設定された回数に達したか判定
    if (currentCount >= targetCount) {
      Serial.write(255);      // Processingへトリガー信号を送信
      currentCount = 0;       // 次の合奏のためにカウントをリセット
      delay(1000);            // 重複送信防止のための待機
    }
  } 
  // 【消灯の検知】
  else if (sensorValue <= threshold && isLightOn == true) {
    isLightOn = false;
    digitalWrite(ledPin, LOW);
    delay(50); // ノイズ対策
  }
}