// === 楽譜データ ===
String []melody = {"C2", "D2", "E2", "F2", "E2", "D2", "C2"}; 
float []duration = {0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 1.2f}; 
float []startTime = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f}; 
float []maxAmp = {0.58f, 0.60f, 0.62f, 0.60f, 0.64f, 0.62f, 0.60f}; 

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
