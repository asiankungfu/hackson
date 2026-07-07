#include <Arduino.h>
#include <Wire.h>
#include "AE_S13683_LED.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;
AE_S13683_LED colorSensor;

// === 1. 点滅検知（フォトトランジスタ）用の設定 ===
const int sensorPin = A0;
const int ledPin = 13;
const int threshold = 300;
const int targetCount = 2;

boolean isLightOn = false;
int currentCount = 0;
unsigned long lastLightTime = 0;
const unsigned long timeout = 500;

// === 2. テンポ変調（カラーセンサ）用の設定 ===
const int brightnessThreshold = 50;
const int maxSensorValue = 800;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(ledPin, OUTPUT);

  Wire.begin();

  if (!colorSensor.begin(&Wire)) {
    while (1);
  }

  colorSensor.colorSensorConfigOneshot(
      AE_S13683_LED::colorSensorGain::GAIN_HIGH,
      20 * 1000);

  colorSensor.ledDriverConfig(false);

  // ===== 歌詞表示初期化 =====
  matrix.begin();
  initLyricsDisplay();
}

void loop() {

  // ===== Processingから送られてくる歌詞番号を処理 =====
  updateLyricDisplay();

  unsigned long currentTime = millis();

  // --------------------------------------------------------
  // 【システム1】点滅の検知とスタート合図
  // --------------------------------------------------------

  int analogLight = analogRead(sensorPin);

  if (analogLight > threshold && !isLightOn) {
    isLightOn = true;
    currentCount++;
    digitalWrite(ledPin, HIGH);

    lastLightTime = currentTime;
    delay(20);
  }
  else if (analogLight <= threshold && isLightOn) {
    isLightOn = false;
    digitalWrite(ledPin, LOW);
    delay(20);
  }

  if (currentCount > 0 && (currentTime - lastLightTime > timeout)) {

    if (currentCount == targetCount) {
      Serial.println(255);
    }

    currentCount = 0;
  }

  // --------------------------------------------------------
  // 【システム2】色によるテンポ変調
  // --------------------------------------------------------

  AE_S13683_LEDResult result =
      colorSensor.getColorSensorResultOneshot();

  int r = result.red;
  int g = result.green;
  int b = result.blue;

  int totalColorLight = r + g + b;

  int tempoSignal = 100;
  if (totalColorLight > brightnessThreshold) {
    if (r > b * 1.3 && r > g * 1.3) {
      // ★ 300 だった部分を 100 に変更し、小さい順（100 -> 800）にする
      tempoSignal = map(r, 100, maxSensorValue, 100, 200);
    }
    else if (b > r * 1.3 && b > g * 1.3) {
      // ★ ここも同様に変更
      tempoSignal = map(b, 100, maxSensorValue, 100, 10);
    }
  }

  tempoSignal = constrain(tempoSignal, 10, 200);

  // ====================================================
  // 現在は歌詞表示との競合を防ぐため送信しない
  // 必要なら通信プロトコルを変更して再度有効化する
  // ====================================================

  Serial.println(tempoSignal);

  delay(20);
}
