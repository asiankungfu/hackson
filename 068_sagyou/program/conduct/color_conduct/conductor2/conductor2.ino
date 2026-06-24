// ピン定義
const int POT_PIN = A0; // 可変抵抗
const int BLUE_PIN = 12; // 青LED（PWM）
const int RED_PIN = 13;  // 赤LED（PWM）
// 緑は使用しない
int lastPotValue = 0;

void setup()
{
  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop()
{
  int potValue = analogRead(POT_PIN); // 0〜1023

  // ノイズ対策（前回より10以上変化があった時だけ処理）
  if (abs(potValue - lastPotValue) > 2)
  {
    lastPotValue = potValue; // 前回の値を更新

    int redValue = 0;
    int blueValue = 0;

    // 【左半分】中央(512)より左にあるとき：青LEDだけを光らせる
    // 【左側】500未満のとき、青を制御（500で完全に消える）
    if (potValue < 500)
    {
      blueValue = map(potValue, 0, 500, 255, 0);
      redValue = 0;
    }
    // 【右側】524より大きいとき、赤を制御（524で完全に消える）
    else if (potValue > 524)
    {
      blueValue = 0;
      redValue = map(potValue, 524, 1023, 0, 255);
    }
    // 【中央】500〜524の間は、どちらも0（完全消灯ゾーン）
    else
    {
      blueValue = 0;
      redValue = 0;
    }

    // LEDに出力
    analogWrite(RED_PIN, redValue);
    analogWrite(BLUE_PIN, blueValue);
  }

  delay(100);
}