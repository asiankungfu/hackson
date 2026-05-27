import ddf.minim.analysis.*;

class WaveVisualizer {
  FFT fft;
  int bands = 32; // バーの数（画面幅に合わせて調整）
  
  WaveVisualizer(AudioOutput out) {
    fft = new FFT(out.bufferSize(), out.sampleRate());
  }

  void drawWave(AudioOutput out) {
    fft.forward(out.mix);
    
    noStroke();
    rectMode(CENTER);
    
    float w = width / (float)bands; // バー1本の幅
    float maxBarHeight = height * 0.7; // 画面の70%を最大高さに制限（オーバーフロー防止）
    
    for (int i = 0; i < bands; i++) {
      // 周波数成分を取得し、画面内に収まるようにマッピング
      // fft.getBand(i) の値を 0 〜 maxBarHeight の範囲に変換
      float amplitude = fft.getBand(i) * 4; 
      float clampedHeight = min(amplitude, maxBarHeight); // 最大高さを超えないよう固定
      
      // ブロックの数を計算（1ブロック12px程度）
      int numBlocks = (int)(clampedHeight / 12);
      
      for (int j = 0; j < numBlocks; j++) {
        // 色をネオングリーンベースに固定
        // 少しだけ明るさを変えて立体感を出す
        fill(0, 255, 150, 150 + j * 5); 
        
        float x = i * w + w/2;
        float y = height - (j * 14) - 20; // 下端から少し浮かせて描画
        
        // 四角いブロックを描画
        rect(x, y, w * 0.7f, 10);
      }
      
      // 最上部のアクセントチップ（より発光しているように見せる）
      if (numBlocks > 0) {
        fill(200, 255, 230); // ほぼ白に近いグリーン
        float topY = height - (numBlocks * 14) - 20;
        rect(i * w + w/2, topY, w * 0.7f, 3);
      }
    }
  }
}
