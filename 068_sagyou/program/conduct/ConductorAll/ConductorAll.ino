#include <Servo.h>

// ---- ピン定義 ----
const int POT_PIN = A0;  // 可変抵抗
const int BLUE_PIN = 3;  // 青LED（PWM）
const int RED_PIN = 5;   // 赤LED（PWM）
const int SERVO_PIN = 6; // サーボモーター

const int LED_PIN = 7;
const int LED_PIN2 = 8;
const int ledPin = 13;                  // 基板のLED
const int BTN_PINS[] = {9, 10, 11, 12}; // A, B, C, D
const int NUM_BTNS = 4;

// ---- BPM・タイミング定数 ----
const int BPM_MIN = 50;
const int BPM_MAX = 170;
const unsigned long BLINK_ON_MS = 150;  // 合図LEDのON時間
const unsigned long BLINK_OFF_MS = 350; // 合図LEDのOFF時間
const int WAIT_BEATS = 8;               // 楽器間の待機拍数

// ---- サーボ角度配列（4拍子パターン） ----
const int BEAT_ANGLES[] = {125, 90, 55, 90}; // 強拍↓ ・ 中央 ・ 弱拍↑ ・ 中央
const int NUM_BEATS = sizeof(BEAT_ANGLES) / sizeof(BEAT_ANGLES[0]);

// ---- グローバル変数 ----
int currentBPM = 120;
unsigned long BEAT_MS = 60000UL / 120;
int lastPotValue = -1; // 初回起動時に必ずLEDを更新させるため -1

int pressOrder[NUM_BTNS];
int pressCount = 0;
bool pressed[NUM_BTNS];

// 指揮者（サーボ）用変数
Servo conductorServo;
int beatIndex = 0;
unsigned long lastBeatTime = 0;

// ---- プロトタイプ宣言 ----
void resetState();
void updatePotentiometer();
void updateConductor();
void blinkLEDAndConduct(int count, unsigned long durationMs);
void conductWaitBeats(int beats);

void setup()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(ledPin, OUTPUT);

    for (int i = 0; i < NUM_BTNS; i++)
    {
        pinMode(BTN_PINS[i], INPUT_PULLUP);
    }

    // サーボ初期設定
    conductorServo.attach(SERVO_PIN);
    conductorServo.write(BEAT_ANGLES[0]);

    // 起動直後にボリューム値を反映
    updatePotentiometer();

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
    // 通常待機時もボリュームを常に監視
    updatePotentiometer();

    // ==========================================
    // ボタン入力のタイミング制御
    // ==========================================
    for (int i = 0; i < NUM_BTNS; i++)
    {
        if (digitalRead(BTN_PINS[i]) == LOW)
        {
            // ボタンが離されるまで待つ（チャタリング防止）
            while (digitalRead(BTN_PINS[i]) == LOW)
            {
                updatePotentiometer();
            }
            delay(20);

            // ボタン入力受付フィードバック
            digitalWrite(ledPin, HIGH);
            delay(80);
            digitalWrite(ledPin, LOW);
            delay(80);

            if (pressed[i])
            {
                resetState();
                return;
            }

            pressed[i] = true;
            pressOrder[pressCount] = i;
            pressCount++;

            // 4つ目のボタンが押された瞬間からタイムラインを開始
            if (pressCount == NUM_BTNS)
            {
                // 指揮の開始タイミングを同期
                lastBeatTime = millis();
                beatIndex = 0;
                conductorServo.write(BEAT_ANGLES[beatIndex]);

                // 1. 各楽器の合図(2000ms) ＋ 8拍の待機 を4回繰り返す
                for (int j = 0; j < NUM_BTNS; j++)
                {
                    int btnIndex = pressOrder[j];
                    int blinkCount = btnIndex + 1; // A=1, B=2, C=3, D=4

                    // ①合図LEDを点滅させつつ、裏で2000msきっちり指揮を振る
                    blinkLEDAndConduct(blinkCount, 2000);

                    // ②次の合図まで 8拍分、指揮を振りながら待機
                    conductWaitBeats(WAIT_BEATS);
                }

                // 2. 追加の演奏時間「34拍」の間、さらに指揮を振り続ける
                conductWaitBeats(34);

                // 全ての指揮・演奏タイムラインが終了したら初期状態へ戻る
                resetState();
            }
            break;
        }
    }
}

// ==========================================
// ボリュームを監視し、1拍ごとに指揮を更新するコア関数
// ==========================================
void updateConductor()
{
    updatePotentiometer(); // ボリューム・BPMのリアルタイム追従

    unsigned long now = millis();

    // 1拍経過するごとにサーボを次の角度へ動かす
    if (now - lastBeatTime >= BEAT_MS)
    {
        lastBeatTime = now;

        // サーボを現在の拍の角度へ移動
        conductorServo.write(BEAT_ANGLES[beatIndex]);

        // 次の拍へ（4拍子のサイクル）
        beatIndex = (beatIndex + 1) % NUM_BEATS;
    }
}

// ==========================================
// 合図LEDを点滅させながら、指定時間(2000ms)指揮を振る関数
// ==========================================
void blinkLEDAndConduct(int count, unsigned long durationMs)
{
    unsigned long startMs = millis();

    for (int j = 0; j < count; j++)
    {
        // LED点灯
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(LED_PIN2, HIGH);

        // 点灯時間(150ms)の間、指揮を更新しながら待つ
        unsigned long onStart = millis();
        while (millis() - onStart < BLINK_ON_MS)
        {
            updateConductor();
        }

        // LED消灯
        digitalWrite(LED_PIN, LOW);
        digitalWrite(LED_PIN2, LOW);

        // 最後の点滅以外は、消灯時間(350ms)の間、指揮を更新しながら待つ
        if (j < count - 1)
        {
            unsigned long offStart = millis();
            while (millis() - offStart < BLINK_OFF_MS)
            {
                updateConductor();
            }
        }
    }

    // 点滅回数が少なくて早く終わった場合、残りの時間を指揮を振りながら埋める（計2000msにする）
    while (millis() - startMs < durationMs)
    {
        updateConductor();
    }
}

// ==========================================
// 指定された「拍数」の分だけ、指揮を振りながら正確に待機する関数
// ==========================================
void conductWaitBeats(int beats)
{
    int count = 0;
    while (count < beats)
    {
        updatePotentiometer(); // ループ内でも随時BPMを読み直す
        unsigned long now = millis();

        // 1拍経過するごとにカウントを進める
        if (now - lastBeatTime >= BEAT_MS)
        {
            lastBeatTime = now;

            // サーボ駆動
            conductorServo.write(BEAT_ANGLES[beatIndex]);
            beatIndex = (beatIndex + 1) % NUM_BEATS;

            count++; // 1拍消化
        }
    }
}

// ==========================================
// ボリューム読み取りとカラーLED・BPM更新
// ==========================================
void updatePotentiometer()
{
    int potValue = analogRead(POT_PIN);

    if (abs(potValue - lastPotValue) > 2)
    {
        lastPotValue = potValue;

        currentBPM = map(potValue, 0, 1023, BPM_MIN, BPM_MAX);
        BEAT_MS = 60000UL / currentBPM;

        int redValue = 0;
        int blueValue = 0;

        if (potValue < 500)
        {
            blueValue = map(potValue, 0, 500, 255, 0);
            redValue = 0;
        }
        else if (potValue > 524)
        {
            blueValue = 0;
            redValue = map(potValue, 524, 1023, 0, 255);
        }
        else
        {
            blueValue = 0;
            redValue = 0;
        }

        analogWrite(RED_PIN, redValue);
        analogWrite(BLUE_PIN, blueValue);
    }
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