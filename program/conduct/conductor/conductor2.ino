// ピン定義
const int POT_PIN = A0; // 可変抵抗
const int BLUE_PIN = 3; // 青LED（PWM）
const int RED_PIN = 5;  // 赤LED（PWM）
// 緑は使用しない

void setup()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
}

void loop()
{
    int potValue = analogRead(POT_PIN); // 0〜1023

    // 赤: 最大(255)→最小(0)
    int redValue = map(potValue, 0, 1023, 255, 0);
    // 青: 最小(0)→最大(255)
    int blueValue = map(potValue, 0, 1023, 0, 255);

    //  analogWrite(RED_PIN,  redValue);
    //  analogWrite(BLUE_PIN, blueValue);

    analogWrite(RED_PIN, 255 - redValue);
    analogWrite(BLUE_PIN, 255 - blueValue);

    delay(10);
}