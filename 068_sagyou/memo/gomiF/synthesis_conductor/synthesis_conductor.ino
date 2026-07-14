// === ピン定義 ===
const int POT_PIN = A0; // 可変抵抗
const int BLUE_PIN = 3; // 青LED
const int RED_PIN = 5;  // 赤LED
const int LED_PIN = 7;  // 点滅用LED
const int BTN_PIN = 12; // ボタン（※12番から2番に変更して競合を回避）

// === 状態管理用の変数 ===
int lastPotValue = 0;   // 前回の可変抵抗の値 [cite: 85]
bool isRunning = false; // 点滅ループが動いているかどうかのフラグ

// delayを使わずにLEDを点滅させるための変数
unsigned long previousMillis = 0;
bool ledState = LOW;

void setup()
{
    pinMode(RED_PIN, OUTPUT);       // [cite: 85]
    pinMode(BLUE_PIN, OUTPUT);      // [cite: 85]
    pinMode(LED_PIN, OUTPUT);       //
    pinMode(BTN_PIN, INPUT_PULLUP); // 内蔵プルアップを使用 [cite: 93]
}

void loop()
{
    // ----------------------------------------------------
    // 1. ボタンの読み取り処理（loopConductorの機能）
    // ----------------------------------------------------
    if (digitalRead(BTN_PIN) == LOW)
    {
        isRunning = !isRunning; // モードを反転（ON <-> OFF） [cite: 94]

        while (digitalRead(BTN_PIN) == LOW)
            ;      // チャタリング・長押し防止 [cite: 94]
        delay(50); // [cite: 94]
    }

    // ----------------------------------------------------
    // 2. 7番ピンLEDの点滅処理（delayを使わない方式）
    // ----------------------------------------------------
    if (isRunning == true)
    {
        unsigned long currentMillis = millis();

        // LEDがHIGHの時は150ms、LOWの時は350ms待つ
        unsigned long interval = (ledState == HIGH) ? 150 : 350;

        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            ledState = !ledState; // LEDの状態を反転
            digitalWrite(LED_PIN, ledState);
        }
    }
    else
    {
        digitalWrite(LED_PIN, LOW); // ストップ時は消灯 [cite: 96]
        ledState = LOW;
    }

    // ----------------------------------------------------
    // 3. 可変抵抗による青・赤LEDの制御（conductor2の機能）
    // ----------------------------------------------------
    int potValue = analogRead(POT_PIN); // 0〜1023を取得 [cite: 85]

    // ノイズ対策（前回より2以上変化があった時だけ処理） [cite: 86]
    if (abs(potValue - lastPotValue) > 2) // [cite: 86]
    {
        lastPotValue = potValue; // 前回の値を更新 [cite: 87]

        int redValue = 0;  // [cite: 87]
        int blueValue = 0; // [cite: 87]

        // 【左側】500未満のとき、青を制御（500で完全に消える） [cite: 88]
        if (potValue < 500) // [cite: 88]
        {
            blueValue = map(potValue, 0, 500, 255, 0); // [cite: 88]
            redValue = 0;                              // [cite: 89]
        }
        // 【右側】524より大きいとき、赤を制御（524で完全に消える） [cite: 89]
        else if (potValue > 524) // [cite: 89]
        {
            blueValue = 0;                               // [cite: 89]
            redValue = map(potValue, 524, 1023, 0, 255); // [cite: 90]
        }
        // 【中央】500〜524の間は、どちらも0（完全消灯ゾーン） [cite: 90]
        else
        {
            blueValue = 0; // [cite: 90]
            redValue = 0;  // [cite: 91]
        }

        // LEDに明るさを出力
        analogWrite(RED_PIN, redValue);   // [cite: 91]
        analogWrite(BLUE_PIN, blueValue); // [cite: 91]
    }

    // 全体のループ速度を少しだけ調整（安定化のため）
    delay(10);
}