import ddf.minim.*;
import ddf.minim.ugens.*;
import ddf.minim.effects.*;
import processing.serial.*;

Minim minim;
AudioOutput out;
Serial myPort;
boolean hasPlayed = false; // 既に演奏したか？を記憶するフラグ

// メロディデータ
String[] melody = { "C3", "D3", "E3", "F3", "E3", "D3", "C3", "E3", "F3", "G3", "A3", "G3", "F3", "E3", "C3", "C3", "C3", "C3", "C3", "C3", "D3", "D3", "E3", "E3", "F3", "F3", "E3", "D3", "C3" };
float[] startTime = { 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 12.25, 12.5, 12.75, 13.0, 13.25, 13.5, 13.75, 14.0, 14.5, 15.0 };
float[] duration = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.5, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.5, 0.5, 1.0 };

void setup() {
  size(600, 300);
  minim = new Minim(this);
  out = minim.getLineOut();
  
  
  // 確認できたArduinoのポート名を直接指定
  String portName = "/dev/cu.usbmodem34B7DA6164342"; 
  
  // 指定したポート名で、Arduino側と同じ通信速度（115200）で接続
  myPort = new Serial(this, portName, 115200);
}

void draw() {
  background(0);
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
}

void playSong() {
  out.pauseNotes(); 
  for (int i = 0; i < melody.length; i++) {
    float freq = Frequency.ofPitch(melody[i]).asHz();
    out.playNote(startTime[i], duration[i], new HackInstrument(freq));
  }
  out.resumeNotes(); 
}

// 完成した倍音特化型・木琴クラス
class HackInstrument implements Instrument {
  Oscil malletClick; 
  ADSR  adsrClick;
  
  HackInstrument(float frequency) {
    float highFreq = frequency * 2.0f;
    malletClick = new Oscil(highFreq * 3.9f, 0.3f, Waves.SINE);
    adsrClick = new ADSR(1.0, 0.002, 0.05, 0.0, 0.05);
    malletClick.patch(adsrClick);
  }
  
  void noteOn(float dur) {
    adsrClick.patch(out);
    adsrClick.noteOn();
  }
  
  void noteOff() {
    adsrClick.noteOff();
    adsrClick.unpatchAfterRelease(out);
  }
}

void keyPressed() {
  if (key == 'r' || key == 'R') {
    hasPlayed = false; // フラグを未演奏に戻す
    println(">>> システムをリセットしました.再び待機します．");
  }
}
