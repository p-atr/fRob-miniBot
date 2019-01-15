#include <FastLED.h>

#define NUM_LEDS 7

#define DATA_PIN 3
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];
int colors[7][3] = {{255,0,0},{170,85,0},{85,170,0},{0,255,0},{0,170,85},{0,85,170},{0,0,255}};

void setup() { 
      FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() { 
  for(int i = 0; i<NUM_LEDS; i++){
    leds[i] = CRGB(colors[i][0],colors[i][1],colors[i][2]);
  }
  FastLED.show();

}
