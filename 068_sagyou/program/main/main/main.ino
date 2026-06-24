#include <Arduino.h>
#include <Wire.h>

#include "AE_S13683_LED.h"

AE_S13683_LED ae_s13683_led;

const int sensorPin = A0; 
const int ledPin = 13;    //基盤のLED
const int threshold = 400;    // テストで確定したしきい値
const int targetCount = 23;    // 担当楽器の起動に必要な点滅回数

///////////////////////色検知用の編集範囲
//const int ColorsensorPin = A1;  // カラーセンサー
//const int Cthreshold = 500;     // カラーセンサーのしきい値（要調整）
//const unsigned long colorSendInterval = 100; // 色データ送信間隔(ms)
//unsigned long lastColorSendTime = 0;
///////////////////////


boolean isLightOn = false;
int currentCount = 0;

void setup() {
  // 設計書通りの高速通信
  Serial.begin(115200);   
  pinMode(ledPin, OUTPUT);

  while (!Serial);

  // I2C初期化
  Wire.begin();

  // カラーセンサー初期化
  ae_s13683_led.begin(&Wire);
  ae_s13683_led.colorSensorConfigAuto();
  Serial.println("start!");
}

void loop() {
  unsigned long now = millis();
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

  ///////////////////////色データ送信
  // 一定間隔でカラーセンサーの値をProcessingへ送信
  //ae_s13683_led.ledDriverConfig(true);
  delay(180);  // 22.4ms * 4色分 * 2周期 ≒ 180
  AE_S13683_LEDResult result = ae_s13683_led.getColorSensorResultAuto();
  //ae_s13683_led.ledDriverConfig(false);
  Serial.print("r: ");
  Serial.print(result.red);
  Serial.print("\tg: ");
  Serial.print(result.green);
  Serial.print("\tb: ");
  Serial.println(result.blue);
  delay(1000);
  ///////////////////////
}