const int LED_PIN = 7;
const int BTN_PIN = 4;

bool isRunning = false; // 点滅ループが動いているかどうかのフラグ

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop() {
  // ボタンが押されたら、モードを切り替える（ON←→OFF）
  if (digitalRead(BTN_PIN) == LOW) {
    isRunning = !isRunning; // trueとfalseを反転
    
    while (digitalRead(BTN_PIN) == LOW); // チャタリング・長押し防止
    delay(50);
  }

  // スタート状態（true）のときだけ点滅する
  if (isRunning == true) {
    digitalWrite(LED_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(350);
  } else {
    digitalWrite(LED_PIN, LOW); // ストップ時は消灯
  }
}