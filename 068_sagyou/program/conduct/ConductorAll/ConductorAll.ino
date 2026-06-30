// ピン定義
const int POT_PIN = A0;  // 可変抵抗
const int BLUE_PIN = 11; // 青LED（PWM）
const int RED_PIN = 12;  // 赤LED（PWM）
// 緑は使用しない
int lastPotValue = 0;

// secondConductor.ino
//
// 4つのボタンを押した順番で演奏順を決定し、
// 4つ目を押した瞬間から順番にLEDで光の合図を送る。
//
// ボタンと点滅回数の対応:
//   A (BTN_PINS[0] = pin4) → 1回点滅
//   B (BTN_PINS[1] = pin3) → 2回点滅
//   C (BTN_PINS[2] = pin2) → 3回点滅
//   D (BTN_PINS[3] = pin5) → 4回点滅

// ---- 定数 ----
const int LED_PIN = 7;
const int ledPin = 13;               // 基盤のLED
const int BTN_PINS[] = {4, 3, 2, 5}; // A, B, C, D
const int NUM_BTNS = 4;
const int BPM = 120; // ここを変えるとテンポが変わる

// BPMから1拍のミリ秒を計算（60000ms / BPM）
const unsigned long BEAT_MS = 60000UL / BPM;

// 点滅1回あたりのON時間とOFF時間（拍に合わせて調整）
const unsigned long BLINK_ON_MS = 150;
const unsigned long BLINK_OFF_MS = 350;

// 各楽器の合図と合図の間の待機拍数
const int WAIT_BEATS = 8;

// ---- グローバル変数 ----
int pressOrder[NUM_BTNS]; // 押された順番にボタンインデックスを記録
int pressCount = 0;       // 現在何回ボタンが押されたか
bool pressed[NUM_BTNS];   // 各ボタンが今回のセッションで押されたか

// ---- プロトタイプ宣言 ----
void resetState();
void blinkLED(int count);
void waitBeats(int beats);

void setup()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    pinMode(LED_PIN, OUTPUT);
    for (int i = 0; i < NUM_BTNS; i++)
    {
        pinMode(BTN_PINS[i], INPUT_PULLUP);
    }
    pinMode(ledPin, OUTPUT);

    // 起動確認用に3回素早く点滅
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(ledPin, HIGH);
        delay(40);
        digitalWrite(ledPin, LOW);
        delay(40);
    }

    resetState();
}

void loop()
{
    /// タイミング///
    for (int i = 0; i < NUM_BTNS; i++)
    {
        if (digitalRead(BTN_PINS[i]) == LOW)
        {

            // ボタンが離されるまで待つ（チャタリング防止）
            while (digitalRead(BTN_PINS[i]) == LOW)
                ;
            delay(20);

            // ボタン入力受付フィードバック（1回点滅）
            digitalWrite(ledPin, HIGH);
            delay(80);
            digitalWrite(ledPin, LOW);
            delay(80);

            // 同じボタンが既に押されていたらリセット
            if (pressed[i])
            {
                resetState();
                return;
            }

            // 押された順番を記録
            pressed[i] = true;
            pressOrder[pressCount] = i;
            pressCount++;

            // 4つ目のボタンが押されたら演奏合図を開始
            if (pressCount == NUM_BTNS)
            {
                // 各楽器に順番に光の合図を送る
                for (int j = 0; j < NUM_BTNS; j++)
                {
                    int btnIndex = pressOrder[j];
                    int blinkCount = btnIndex + 1; // A=1, B=2, C=3, D=4

                    unsigned long startMs = millis(); // 点滅開始時刻を記録
                    blinkLED(blinkCount);
                    if (j < NUM_BTNS - 1)
                    {
                        unsigned long elapsed = millis() - startMs;
                        unsigned long interval = BEAT_MS * WAIT_BEATS;
                        if (elapsed < interval)
                        {
                            delay(interval - elapsed); // 残り時間だけ待つ → 均等拍
                        }
                    }
                }

                // 演奏合図が全て終わったら待機状態に戻る
                resetState();
            }

            // 同一ループ内で複数ボタンの同時検出を防ぐ
            break;
        }
    }

    /// カラー///
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

// ---- 状態リセット ----
void resetState()
{
    pressCount = 0;
    for (int i = 0; i < NUM_BTNS; i++)
    {
        pressOrder[i] = -1;
        pressed[i] = false;
    }
}

// ---- count回LEDを点滅させる ----
void blinkLED(int count)
{
    for (int j = 0; j < count; j++)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(BLINK_ON_MS);
        digitalWrite(LED_PIN, LOW);

        // 最後の点滅の後はOFF時間を入れない
        if (j < count - 1)
        {
            delay(BLINK_OFF_MS);
        }
    }
}

// ---- beats拍分だけ待機する ----
void waitBeats(int beats)
{
    delay(BEAT_MS * beats);
}
