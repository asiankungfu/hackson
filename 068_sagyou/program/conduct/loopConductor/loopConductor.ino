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
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
}