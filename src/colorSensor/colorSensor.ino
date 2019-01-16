#include "Wire.h"
#include "veml6040.h"
VEML6040 RGBWSensor;

void veml_setup() {
  Wire.begin();
  if(!RGBWSensor.begin()) {
    Serial.println("ERROR: couldn't detect VEML6040");
    while(1){}
  }
  RGBWSensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  long r = RGBWSensor.getRed()/256;
  long g = RGBWSensor.getGreen()/256;
  long b = RGBWSensor.getBlue()/256;
  //Serial.println(String(r) + ", " + String(g) + ", " + String(b) + ", " + String(w));
  if(r > g){
    Serial.println("Left");
  } else if(b > g){
    Serial.println("Right");
  } else{
    Serial.println("Stay");
  }

}
