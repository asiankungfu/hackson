// kaeru_all_1.ino
//
// 前身コード（点滅検知＋カラーセンサ）と
// 現行コード（LEDマトリクス歌詞表示＋テンポ変調）を統合し、
// 以下のバグを修正：
//   - lastLightTimeをOFF時刻に修正
//   - カラーセンサのブロッキングを末尾に分離
//   - 重複送信防止をdelayからフラグに変更
//   - tempoSignalのマジックナンバーを定数化

#include <Arduino.h>
#include <Wire.h>
#include "AE_S13683_LED.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;
AE_S13683_LED colorSensor;

// ---- 点滅検知（フォトトランジスタ）用の設定 ----
const int sensorPin = A0;
const int ledPin = 13;     // 基盤のLED
const int threshold = 400; // テストで確定したしきい値
const int targetCount = 2; // 担当楽器の起動に必要な点滅回数（要仕様確認）

const unsigned long BLINK_TIMEOUT_MS = 500; // 点滅セッション終了とみなすOFF持続時間

boolean isLightOn = false;
int currentCount = 0;
bool triggered = false;        // 重複送信防止フラグ
unsigned long lastOffTime = 0; // 最後に光がOFFになった時刻（タイムアウト判定用）

// ---- テンポ変調（カラーセンサ）用の設定 ----
const int brightnessThreshold = 300; // 色検知を有効にする総輝度の下限
const int COLOR_SENSOR_MIN = 300;    // map()の入力下限（brightnessThresholdと統一）
const int COLOR_SENSOR_MAX = 1000;   // map()の入力上限
const int TEMPO_NEUTRAL = 100;       // 色検知なし時のデフォルトテンポ信号
const int TEMPO_RED_MAX = 200;       // 赤検知時のテンポ上限
const int TEMPO_BLUE_MIN = 10;       // 青検知時のテンポ下限

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    pinMode(ledPin, OUTPUT);

    Wire.begin();

    if (!colorSensor.begin(&Wire))
    {
        // カラーセンサ初期化失敗：基板LEDを高速点滅してエラー通知
        while (1)
        {
            digitalWrite(ledPin, HIGH);
            delay(100);
            digitalWrite(ledPin, LOW);
            delay(100);
        }
    }

    // Oneshotモード（積分時間20ms）
    // ※ ブロッキングが発生するためloop()末尾でのみ呼ぶこと
    colorSensor.colorSensorConfigOneshot(
        AE_S13683_LED::colorSensorGain::GAIN_HIGH,
        20 * 1000);
    colorSensor.ledDriverConfig(false);

    matrix.begin();
    initLyricsDisplay(); // 別ファイルで定義
}

void loop()
{
    unsigned long now = millis();

    // --------------------------------------------------------
    // 【システム1】点滅の検知とスタート合図
    // --------------------------------------------------------
    int analogLight = analogRead(sensorPin);

    // 暗→明：点滅カウントアップ
    if (analogLight > threshold && !isLightOn)
    {
        isLightOn = true;
        currentCount++;
        digitalWrite(ledPin, HIGH);
        delay(20); // チャタリング防止

        if (currentCount >= targetCount && !triggered)
        {
            Serial.write(255); // Processingへトリガー信号を送信
            triggered = true;
            currentCount = 0;
            // ※ delay(1000)は削除。triggeredフラグで重複送信を防止。
            // triggered のリセットはタイムアウト判定でまとめて行う。
        }
    }
    // 明→暗：消灯検知。lastOffTimeをここで更新する（修正箇所）
    else if (analogLight <= threshold && isLightOn)
    {
        isLightOn = false;
        lastOffTime = now; // OFFになった時刻を記録（旧コードはON時刻を誤って記録していた）
        digitalWrite(ledPin, LOW);
        delay(20); // チャタリング防止
    }

    // OFFが一定時間続いたら点滅セッション終了とみなしリセット
    if (!isLightOn && lastOffTime > 0 && (now - lastOffTime > BLINK_TIMEOUT_MS))
    {
        currentCount = 0;
        triggered = false; // 次の合奏に備えてフラグをリセット
        lastOffTime = 0;
    }

    // --------------------------------------------------------
    // 【システム2】歌詞表示の更新
    // --------------------------------------------------------
    updateLyricDisplay(); // 別ファイルで定義

    // --------------------------------------------------------
    // 【システム3】色によるテンポ変調
    // カラーセンサはブロッキング（約20ms）のためloop末尾に配置
    // --------------------------------------------------------
    delay(180); // 22.4ms × 4色 × 2周期 ≒ 180ms（センサ安定待ち）
    AE_S13683_LEDResult result = colorSensor.getColorSensorResultOneshot();

    int r = result.red;
    int g = result.green;
    int b = result.blue;
    int totalColorLight = r + g + b;

    int tempoSignal = TEMPO_NEUTRAL;

    if (totalColorLight > brightnessThreshold)
    {
        if (r > b * 1.3 && r > g * 1.3)
        {
            // 赤優勢：テンポを上げる
            tempoSignal = map(r, COLOR_SENSOR_MIN, COLOR_SENSOR_MAX, TEMPO_NEUTRAL, TEMPO_RED_MAX);
        }
        else if (b > r * 1.3 && b > g * 1.3)
        {
            // 青優勢：テンポを下げる
            tempoSignal = map(b, COLOR_SENSOR_MIN, COLOR_SENSOR_MAX, TEMPO_NEUTRAL, TEMPO_BLUE_MIN);
        }
    }

    tempoSignal = constrain(tempoSignal, TEMPO_BLUE_MIN, TEMPO_RED_MAX);

    // ※ 現在はProcessingとのシリアル通信プロトコル未整合のため送信無効
    // トリガー信号(255)と共存できるプロトコルに変更後、下行を有効化すること
    // Serial.println(tempoSignal);

    delay(20);
}
