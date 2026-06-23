import ddf.minim.*;
import ddf.minim.ugens.*;
import ddf.minim.effects.*;
import processing.serial.*;

Minim minim;
AudioOutput out;
Serial myPort;        // シリアル通信用
boolean hasPlayed = false; // 既に演奏したか？を記憶するフラグ

// テンポ制御
float currentTempo = 120;  // 現在のテンポ(BPM)
float targetTempo  = 120;  // カラーセンサーから指示されたテンポ

// カラーセンサー設定
final int COLOR_MIN   = 0;    // センサー最小値（青に相当）
final int COLOR_MAX   = 1023; // センサー最大値（赤に相当）
final int COLOR_MID   = 512;  // 無色（通常）に相当する中央値
final float TEMPO_MIN = 60;   // 青のとき最低テンポ
final float TEMPO_MID = 120;  // 無色のとき通常テンポ
final float TEMPO_MAX = 200;  // 赤のとき最高テンポ

void setup() { 
  size(600, 300);     // ウィンドウサイズ統一
  pixelDensity(1);    // 警告対策：高解像度画面での表示ズレを防止
  
  minim = new Minim(this);
  out = minim.getLineOut();

  // カエル側で成功したポート名（/dev/cu.usbmodem34B7DA6206942など）にここを合わせます
  String portName = "/dev/cu.usbmodemF412FA9FADA42"; 
  
  // 指定したポート名で接続
  myPort = new Serial(this, portName, 115200);
  
  out.setTempo(120); 
  tromboneWave = WavetableGenerator.gen10(
    4096,
    new float[] {1.0f, 0.0f, 0.32f, 0.2f, 0.1f}
  );
  
  myPort.bufferUntil('\n'); // '\n' を受信したら serialEvent を呼ぶ
}

void draw() {
  background(0);

  // テンポをなめらかに追従（毎フレーム 1BPM ずつ近づける）
  if (abs(currentTempo - targetTempo) > 1.0) {
    currentTempo += (targetTempo - currentTempo) * 0.05;
    out.setTempo(currentTempo);
  }

  // ─── テンポに応じた色 ───
  // 青(遅)→白(中)→赤(早) でグラデーション表示
  float t = map(currentTempo, TEMPO_MIN, TEMPO_MAX, 0, 1);
  color barColor;
  if (t < 0.5) {
    // 青 → 白
    float f = t * 2;
    barColor = color(lerp(50, 255, f), lerp(100, 255, f), lerp(255, 255, f));
  } else {
    // 白 → 赤
    float f = (t - 0.5) * 2;
    barColor = color(lerp(255, 255, f), lerp(255, 80, f), lerp(255, 80, f));
  }

  // テンポバー（画面下部）
  noStroke();
  fill(40);
  rect(0, height - 30, width, 30);
  fill(barColor);
  rect(0, height - 30, width * t, 30);

  // テキスト表示
  fill(255);
  textSize(14);
  text("Tempo: " + nf(currentTempo, 0, 1) + " BPM", 10, height - 10);

  if (hasPlayed == false) {
    fill(200);
  } else {
    fill(255, 120, 120);
  }
  textSize(14);
  text(hasPlayed ? "Status: Played  [R] to Reset" : "Status: Waiting for Arduino...", 10, 20);
}

// シリアル信号割り込み
void serialEvent(Serial p) {
  String line = p.readStringUntil('\n');
  if (line == null) return;
  line = trim(line);

  // ── カラーセンサーデータ「C:<値>」 ──
  if (line.startsWith("C:")) {
    int colorVal = int(line.substring(2));
    colorVal = constrain(colorVal, COLOR_MIN, COLOR_MAX);

    // 中央(512)を境に青→通常→赤へマッピング
    if (colorVal <= COLOR_MID) {
      // 青側: 値が小さいほど遅い
      targetTempo = map(colorVal, COLOR_MIN, COLOR_MID, TEMPO_MIN, TEMPO_MID);
    } else {
      // 赤側: 値が大きいほど早い
      targetTempo = map(colorVal, COLOR_MID, COLOR_MAX, TEMPO_MID, TEMPO_MAX);
    }
    println("Color=" + colorVal + "  → targetTempo=" + nf(targetTempo, 0, 1));
    return;
  }

  // ── トリガー信号（バイト値 255）──
  // bufferUntil('\n') 使用中はバイナリ 255 が来たとき readString が空になるため
  // 生バイトも別途チェックする
  if (p.available() > 0) {
    int inByte = p.read();
    if (inByte == 255 && hasPlayed == false) {
      println("START received");
      playSong();
      hasPlayed = true;
    } else if (inByte == 255) {
      println("演奏中なのでSTARTを無視しました");
    }
  }
}

// キーボード操作もカエル側の確実なロジックに統一
void keyPressed() {
  if (key == 'p' || key == 'P') {
    if (hasPlayed == false) {
      playSong();
      hasPlayed = true;
      println(">>> 手動で再生しました。");
    } else {
      println("演奏中なのでP入力を無視しました");
    }
  }
  
  if (key == 'r' || key == 'R') {
    resetPlayback();
  }
}

// 安全にリセットするための関数
void resetPlayback() {
  out.close();
  out = minim.getLineOut();
  currentTempo = 120;
  targetTempo  = 120;
  out.setTempo(currentTempo);
  hasPlayed = false;
  println(">>> 演奏を停止し、再び待機します。");
}
