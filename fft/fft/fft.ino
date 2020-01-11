#include<ArduinoSound.h>

const int sampleRate= 8000;

const int fftSize = 128;

const int spectrumSize = fftSize/2;

const df = sampleRate/fftSize;

int spectrum[spectrumSize];

FFTAnalyzer fftAnalyzer(fftSize);
1111

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) {
    ;
  }

  if(!AudioInI2S.begin(sampleRate, 32){ // 32 bit per sample
    Serial.print("Failedto init I2S");
  }

  if(!fftAnalyzer.input(AudioInI2S)) {
    Serial.println("Failed to set FFT analyzer"); 
  }
}

void loop() {
  
  if(fftAnalyzer.available() {
    fftAnalyzer.read(spectrum, spectrumSize);
    for(int i=0; i<spectrumSize; i++) {
      Serial.println( (i*df));
      Serial.print("\t");
      Serial.println(spectrum[i]);
    }
  }
}
