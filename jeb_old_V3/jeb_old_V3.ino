#include <FastLED.h> // Bibliotheque qui gere les leds adressables

#define NUM_LEDS 33 // Nombe total de leds sur la section connectee au DATA_PIN
#define DATA_PIN 4 // Pin numerique DATA de la bande led W2812B
#define POT1 A1 // Pin analogique du potentiometre 1 - Duree de cycle HUE
#define POT2 A2 // Pin analogique du potentiometre 2 - Intensite lumineuse des ledsj

CRGB leds[NUM_LEDS]; // Initialisation variable leds
uint8_t hue = 0; // Initialisation du cycle HUE (arc-en-ciel) a la valeur 0

void setup() { 
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // Alt : RGB | Normal : GRB - Declaration des leds
    
    pinMode(POT1, INPUT); // Potentiometre 1
    pinMode(POT2, INPUT); // Potentiometre 2
    
}

void jeb(){
  
      for (int i = 0; i < 33; i++) {
       leds[i] = CHSV(hue, 255, 255);
    }
  
    leds[15] = CRGB(255,90,110);
    leds[16] = CRGB(255,90,110);
    leds[18] = CRGB(255,90,110);
    leds[19] = CRGB(255,90,110);
    leds[21] = CRGB(255,90,110);
    leds[22] = CRGB(255,90,110);    
    leds[24] = CRGB(255,90,110);
    leds[25] = CRGB(255,90,110);
     
    hue++; // Incrementation HUE (cycle de couleur arc-en-ciel RGB)
    
}

void loop() {
  if (analogRead(POT2) > 895){
    
      for (int i = 0; i < 33; i++) {
       leds[i] = CRGB(255, 255, 255);
    }
    FastLED.delay(25);
  
  }
  else if (analogRead(POT2) > 767){
    
    jeb();
    leds[32] = CRGB(255,0,0);
    FastLED.delay(10); // Duree cycle HUE (Potentiometre 1)
    
  }
  else if (analogRead(POT2) > 639){
    
    jeb();
    leds[32] = CRGB(0,255,0);
    FastLED.delay(15); // Duree cycle HUE (Potentiometre 1)
    
  }
  else if (analogRead(POT2) > 511){
    
    jeb();
    leds[32] = CRGB(0,0,255);
    FastLED.delay(20); // Duree cycle HUE (Potentiometre 1)
    
  }
  else if (analogRead(POT2) > 383){
    
    jeb();
    leds[32] = CRGB(255,255,0);
    FastLED.delay(30); // Duree cycle HUE (Potentiometre 1)
    
  }
  else if (analogRead(POT2) > 255){
    
    jeb();
    leds[32] = CRGB(0,255,255);
    FastLED.delay(40); // Duree cycle HUE (Potentiometre 1)
    
  }
  else if (analogRead(POT2) > 127){
    
    jeb();
    leds[32] = CRGB(255,0,255);
    FastLED.delay(55); // Duree cycle HUE (Potentiometre 1)
    
  }
  else if (analogRead(POT2) > 0){
    
    jeb();
    leds[32] = CRGB(255,255,255);
    FastLED.delay(75); // Duree cycle HUE (Potentiometre 1)
    
  }
}
/*
void loop() {
  if (analogRead(POT1) > 895){
    
    LEDS.setBrightness(255)
  
  }
  else if (analogRead(POT1) > 767){
    
    LEDS.setBrightness(200)
    
  }
  else if (analogRead(POT1) > 639){
    
    LEDS.setBrightness(150)
    
  }
  else if (analogRead(POT1) > 511){
    
    LEDS.setBrightness(100)
    
  }
  else if (analogRead(POT1) > 383){
    
    LEDS.setBrightness(60)
    
  }
  else if (analogRead(POT1) > 255){
    
    LEDS.setBrightness(40)
    
  }
  else if (analogRead(POT1) > 127){
    
    LEDS.setBrightness(20)
    
  }
  else if (analogRead(POT1) > 0){
    
    LEDS.setBrightness(10)
    
  }

}
*/
