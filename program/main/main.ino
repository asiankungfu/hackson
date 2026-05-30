const int sensorPin = A0;
const int ledPin = 13;
const int threshold = 600; // テストで確定したしきい値
const int targetCount = 6; // 担当楽器の起動に必要な点滅回数

boolean isLightOn = false;
int currentCount = 0;
unsigned long lastLightTime = 0;
const unsigned long timeout = 1000;

void setup()
{
    // 設計書通りの高速通信
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
}

void loop()
{
    int sensorValue = analogRead(sensorPin);
    unsigned long currentTime = millis(); // 現在の時刻を取得

    // 【点滅の検知】暗状態から明状態への変化を捉える
    if (sensorValue > threshold && isLightOn == false)
    {
        isLightOn = true;
        currentCount++;
        digitalWrite(ledPin, HIGH); // 検知した瞬間に基板LEDを点灯

        lastLightTime = currentTime;

        delay(50); // 二重カウント防止
    }
    // 【消灯の検知】
    else if (sensorValue <= threshold && isLightOn == true)
    {
        isLightOn = false;
        digitalWrite(ledPin, LOW);
        delay(50); // ノイズ対策
    }

    // 【まとまりの終了判定】光が途絶えてから一定時間（timeout）経過したか？
    // かつ、1回以上カウントされている場合のみ処理する
    if (currentCount > 0 && (currentTime - lastLightTime > timeout))
    {

        // 一連の点滅が終わった！最終的なカウント数が自分の目標と一致しているか？
        if (currentCount == targetCount)
        {
            Serial.write(255); // 一致していればProcessingへ送信！
                               // 連続発火を防ぐための待機が必要な場合はここに入れる
                               // delay(1000);
        }

        // 一致していてもしていなくても、判定が終わったらカウントを0に戻す
        currentCount = 0;
    }
}