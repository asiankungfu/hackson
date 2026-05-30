// 現状実行部とsynth部を分けている

import ddf.minim.*;
import ddf.minim.ugens.*;

//よくわからんが追加

import ddf.minim.effects.*;
import processing.serial.*;



Minim minim;
AudioOutput out;
WaveVisualizer vis;   //ビジュアライザー用　追加
Serial myPort;        //シリアル通信用　追加
boolean hasPlayed = false; // 既に演奏したか？を記憶するフラグ

void setup() {
  size(600, 300);     //ウィンドウサイズ統一
  minim = new Minim(this);
  out = minim.getLineOut();

  // ★変更点：確認できたArduinoのポート名を直接指定します
  String portName = "自分のやっといて"; 
  
  // 指定したポート名で、Arduino側と同じ通信速度（115200）で接続
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
  vis.drawWave(out); 


//下記追加

  stroke(0, 255, 0);
  strokeWeight(2);
  for(int i = 0; i < out.bufferSize() - 1; i++) {
    line(i, 150 + out.left.get(i)*100, i+1, 150 + out.left.get(i+1)*100);
  }
  fill(255);
  
  if (hasPlayed == false) {
  } else {
    fill(255, 100, 100); // 再生後は赤文字にする
  }

  // 受信処理
  while (myPort.available() > 0) {
    int inByte = myPort.read();
    
    // 255が来て，かつまだ演奏していない時だけ鳴らす
    if (inByte == 255 && hasPlayed == false) {
      playSong();
      hasPlayed = true; 
    } 
    else if (inByte == 255 && hasPlayed == true) {
    }
  }
//ここまで

}

void keyPressed() {
  if (key == 'p') playSong();
}


// シリアル信号割り込み（Arduinoからデータが来た瞬間に呼ばれる）
void serialEvent(Serial p) {
  int inByte = p.read();
  
  // Arduinoからトリガー「255」を受け取ったら演奏スタート
  if (inByte == 255) {
    playSong();
  }
}

//リセット用
void keyPressed() {
  if (key == 'r' || key == 'R') {
    hasPlayed = false; // フラグを未演奏に戻す
    println(">>> システムをリセットしました.再び待機します．");
  }
}