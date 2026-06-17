// === カエルの歌（tu-ba音域）楽譜データ ===
String []melody = {
  // かえるのうたが 〜 きこえてくるよ
  "C2", "D2", "E2", "F2", "E2", "D2", "C2", "",
  "E2", "F2", "G2", "A2", "G2", "F2", "E2", "",
  
  // クワクワクワクワ
  "C2", "", "C2", "", "C2", "", "C2", "",
  
  // ケケケケ ケケケケ 〜 クワクワクワ
  "C2", "C2", "D2", "D2", "E2", "E2", "F2", "F2",
  "E2", "D2", "C2"
}; 

float []duration = {
  // かえるのうたが 〜 きこえてくるよ
  0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 1.2f, 0.5f,
  0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 1.2f, 0.5f,
  
  // クワクワクワクワ
  0.9f, 0.5f, 0.9f, 0.5f, 0.9f, 0.5f, 0.9f, 0.5f,
  
  // ケケケケ ケケケケ 〜 クワクワクワ
  0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f,
  0.9f, 0.9f, 3.0f
}; 

float []startTime = {
  // かえるのうたが 〜 きこえてくるよ
  0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.5f,
  8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.5f,
  
  // クワクワクワクワ
  16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f,
  
  // ケケケケ ケケケケ 〜 クワクワクワ
  24.0f, 24.5f, 25.0f, 25.5f, 26.0f, 26.5f, 27.0f, 27.5f,
  28.0f, 29.0f, 30.0f
}; 

float []maxAmp = {
  // かえるのうたが 〜 きこえてくるよ
  0.58f, 0.60f, 0.62f, 0.60f, 0.64f, 0.62f, 0.60f, 0.00f,
  0.60f, 0.62f, 0.65f, 0.66f, 0.65f, 0.62f, 0.60f, 0.00f,
  
  // クワクワクワクワ
  0.58f, 0.00f, 0.58f, 0.00f, 0.58f, 0.00f, 0.58f, 0.00f,
  
  // ケケケケ ケケケケ 〜 クワクワクワ
  0.55f, 0.55f, 0.56f, 0.56f, 0.58f, 0.58f, 0.58f, 0.58f,
  0.62f, 0.60f, 0.58f
};

Waveform tromboneWave; 

// === 楽器クラス ===
class TromboneInstrument implements Instrument 
{
  Oscil wave; 
  ADSR  adsr; 

  TromboneInstrument(float frequency, float maxAmp, Waveform wf) 
  {
    wave = new Oscil(frequency, 1.0f, wf); 
    adsr = new ADSR(maxAmp, 0.15, 0.1, 0.8, 0.2); 
    wave.patch(adsr); 
  }

  void noteOn(float duration) 
  {
    adsr.noteOn(); 
    adsr.patch(out); 
  }

  void noteOff() 
  {
    adsr.noteOff(); 
    adsr.unpatchAfterRelease(out); 
  }
}

// === 再生関数 ===
void playSong() 
{
  out.pauseNotes(); 

  for (int i = 0; i < melody.length; i++) { 
    out.playNote(
      startTime[i],
      duration[i],
      new TromboneInstrument(
        Frequency.ofPitch(melody[i]).asHz(),
        maxAmp[i],
        tromboneWave
      )
    ); 
  }

  out.resumeNotes(); 
}
