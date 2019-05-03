#include <FastLED.h>

#define NUM_LEDS 7
#define DATA_PIN 3
#define CLOCK_PIN 13

CRGB leds[NUM_LEDS];
int led_colors[7][3] = {{255,0,0},{170,85,0},{85,170,0},{0,255,0},{0,170,85},{0,85,170},{0,0,255}};

void led_setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  for(int i = 0; i<NUM_LEDS; i++){
    leds[i] = CRGB(led_colors[i][0],led_colors[i][1],led_colors[i][2]);
  }
  FastLED.show();
}

void setup() {
  led_setup();
}

void loop() {
}
