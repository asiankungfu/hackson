// 予約語との衝突を避けるため、名前を変更します
const int LED_PIN = 7;
const int NUM_BTNS = 4;

void setup()
{
    // 7番ピンを出力モードに設定
    pinMode(LED_PIN, OUTPUT);
}

void loop()
{
    for (int i = 0; i < NUM_BTNS; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(0); // 少し長めにして、開始合図を明確にする

        // 2. 楽器番号に応じた点滅処理
        // blinkLED関数の内部で「何回点滅するか」を制御する想定です
        blinkLED(i + 1);

        // 3. 次のループへの準備（消灯して間隔を空ける）
        digitalWrite(LED_PIN, LOW);
        delay(4000);
    }
}

void blinkLED(int count)
{
    for (int j = 0; j < count; j++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(250);
        digitalWrite(LED_PIN, LOW);

        if (j < count - 1)
        {
            delay(500);
        }
    }
}