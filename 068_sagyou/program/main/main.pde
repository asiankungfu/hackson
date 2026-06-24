import ddf.minim.*;
import ddf.minim.ugens.*;
import ddf.minim.effects.*;
import processing.serial.*;

Minim minim;
AudioOutput out;
//WaveVisualizer vis;   // ビジュアライザー用
Serial myPort;        // シリアル通信用
boolean hasPlayed = false; // 既に演奏したか？を記憶するフラグ

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
  
  vis = new WaveVisualizer(out);
}

void draw() {
  background(0);
  // 「音の担当」が鳴らしている out を、「見た目の担当」に渡して描画
  //vis.drawWave(out); 

  // 波形描画
  //stroke(0, 255, 0);
  //strokeWeight(2);
  //for(int i = 0; i < out.bufferSize() - 1; i++) {
  //  line(i, 150 + out.left.get(i)*100, i+1, 150 + out.left.get(i+1)*100);
  //}
  
  // 状態に合わせて文字色を変更
  if (hasPlayed == false) {
    fill(255);
  } else {
    fill(255, 100, 100); // 再生後は赤文字にする
  }
  
  // 目標回数表示を 5 flashes に修正
  text(hasPlayed ? "Status: Played (Press 'R' to Reset)" : "Status: Waiting for Arduino (5 flashes)", 20, 30);
}

// シリアル信号割り込み（実行可能コードと同じロジックに統一）
void serialEvent(Serial p) {
  int inByte = p.read();
  println("received byte: " + inByte);

  if (inByte == 255 && hasPlayed == false) {
    println("START received");
    playSong();
    hasPlayed = true; 
  } 
  else if (inByte == 255 && hasPlayed == true) {
    println("演奏中なのでSTARTを無視しました");
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
  out.setTempo(120);
  hasPlayed = false;
  println(">>> 演奏を停止し、再び待機します。");
}
